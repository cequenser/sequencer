#pragma once

#include <sequencer/beat_duration.hpp>
#include <sequencer/midi/device_spec.hpp>
#include <sequencer/midi/device_spec_reader.hpp>
#include <sequencer/midi/lfo.hpp>
#include <sequencer/midi/message/channel_voice.hpp>
#include <sequencer/midi/track_parameter.hpp>

#include <algorithm>
#include <array>
#include <cassert>
#include <fstream>
#include <limits>
#include <regex>

namespace sequencer::backend::digitakt
{
    inline std::array< double, 128 > create_length_map()
    {
        std::array< double, 128 > length_map;
        using size_type = std::array< double, 128 >::size_type;
        auto value = 0.125;
        size_type start = 0;
        size_type end = 30;
        const auto add_values = [&]( auto delta ) {
            for ( size_type i = start; i < end; ++i )
            {
                length_map[ i ] = value / 4;
                value += delta;
            }
            start = end;
            end = start + 16;
        };

        add_values( 0.0625 );
        add_values( 0.125 );
        add_values( 0.25 );
        add_values( 0.5 );
        add_values( 1.0 );
        add_values( 2.0 );
        add_values( 4.0 );
        length_map[ 127 ] = std::numeric_limits< double >::max();
        return length_map;
    }

    inline const std::array< double, 128 >& length_map()
    {
        static const auto length_map = create_length_map();
        return length_map;
    }

    struct device_spec_cc_t
    {
        std::vector< std::vector< midi::device_entry_t > > control_parameter;
    };

    template < class >
    class lfo_t;

    struct track_parameter_t : midi::track_parameter_t
    {
        enum idx
        {
            trig = 0,
            source,
            filter,
            amp,
            lfo,
            delay,
            reverb,
            count
        };

        static constexpr auto note_offset_idx = 0u;
        static constexpr auto velocity_idx = 1u;
        static constexpr auto length_idx = 2u;
        enum lfo_idx
        {
            speed = 0,
            multiplier,
            fade,
            destination,
            wave_form,
            phase,
            mod,
            depth
        };

        enum wave_from_idx
        {
            triangular = 0,
            sine,
            square,
            saw,
            exp,
            ramp,
            random
        };

        track_parameter_t() : midi::track_parameter_t{idx::count, length_idx + 1}
        {
        }

        auto note_offset() const noexcept
        {
            return values[ idx::trig ][ note_offset_idx ].load();
        }

        auto velocity() const noexcept
        {
            return values[ idx::trig ][ velocity_idx ].load();
        }

        void set_velocity( std::uint16_t velocity ) noexcept
        {
            values[ idx::trig ][ velocity_idx ] = velocity;
        }

        beat_duration note_length() const noexcept
        {
            return beat_duration{length_map()[ values[ idx::trig ][ length_idx ].load() ]};
        }

        void set_note_length_idx( std::uint16_t idx ) noexcept
        {
            values[ idx::trig ][ length_idx ] = idx;
        }

        auto lfo_destination() const noexcept
        {
            return values[ idx::lfo ][ lfo_idx::destination ];
        }

        auto lfo_speed() const noexcept
        {
            return values[ idx::lfo ][ lfo_idx::speed ] * values[ idx::lfo ][ lfo_idx::multiplier ];
        }

        bool lfo_enabled() const noexcept
        {
            return lfo_destination() > 0 && lfo_speed() != 0;
        }

        template < class F >
        auto lfo_func( std::uint8_t min, std::uint8_t max, F f ) const noexcept
        {
            return lfo_t< F >{min, max, f, values[ idx::lfo ]};
        }
    };

    template < class F >
    class lfo_t
    {
    public:
        explicit lfo_t( std::uint8_t min, std::uint8_t max, F f,
                        const midi::track_parameter_t::parameters_type& parameters ) noexcept
            : min_{min}, max_{max}, f_{f}, parameters_{parameters}
        {
        }

        std::optional< midi::message_t< 3 > > operator()( std::size_t pulses_per_quarter_note,
                                                          bool restart, bool note_send ) const
            noexcept
        {
            if ( restart || ( lfo_trig_mode() && note_send ) )
            {
                pulse_count_ = 0;
                if ( restart )
                {
                    return {};
                }
            }

            if ( lfo_hold_mode() && !note_send )
            {
                ++pulse_count_;
                return {};
            }

            const auto period_length = 4 * pulses_per_quarter_note * 128 / lfo_speed();
            if ( ( lfo_one_mode() && pulse_count_ > period_length ) ||
                 ( lfo_half_mode() && pulse_count_ > period_length / 2 ) )
            {
                return {};
            }
            if ( ( lfo_one_mode() && pulse_count_ == period_length ) ||
                 ( lfo_half_mode() && pulse_count_ == period_length / 2 ) )
            {
                ++pulse_count_;
                return f_( ( min_ + max_ ) / 2 + ( min_ + max_ ) % 2 );
            }

            const auto lfo_value = midi::lfo< std::uint8_t >(
                pulse_count_++, pulses_per_quarter_note, lfo_speed(), lfo_phase(), min_, max_,
                static_cast< midi::lfo_mode >(
                    parameters_[ track_parameter_t::lfo_idx::wave_form ].load() ) );
            return f_( lfo_value );
        }

    private:
        bool lfo_trig_mode() const noexcept
        {
            return parameters_[ track_parameter_t::lfo_idx::mod ] == 1;
        }

        bool lfo_hold_mode() const noexcept
        {
            return parameters_[ track_parameter_t::lfo_idx::mod ] == 2;
        }

        bool lfo_one_mode() const noexcept
        {
            return parameters_[ track_parameter_t::lfo_idx::mod ] == 3;
        }

        bool lfo_half_mode() const noexcept
        {
            return parameters_[ track_parameter_t::lfo_idx::mod ] == 4;
        }

        auto lfo_phase() const noexcept
        {
            return parameters_[ track_parameter_t::lfo_idx::phase ];
        }

        auto lfo_speed() const noexcept
        {
            return parameters_[ track_parameter_t::lfo_idx::speed ] *
                   parameters_[ track_parameter_t::lfo_idx::multiplier ];
        }

        std::uint8_t min_;
        std::uint8_t max_;
        F f_;
        const midi::track_parameter_t::parameters_type& parameters_;
        mutable std::size_t pulse_count_{0};
    };

#define READ_SECTION( section_name )                                                               \
    if ( section.name == #section_name )                                                           \
    {                                                                                              \
        device_spec.control_parameter[ track_parameter_t::idx::section_name ].resize(              \
            section.entries.size() );                                                              \
        std::copy(                                                                                 \
            begin( section.entries ), end( section.entries ),                                      \
            begin( device_spec.control_parameter[ track_parameter_t::idx::section_name ] ) );      \
        continue;                                                                                  \
    }

    inline device_spec_cc_t read_device_spec_cc( std::istream& stream )
    {
        const auto sections = midi::read_file( stream );

        device_spec_cc_t device_spec;
        device_spec.control_parameter.resize( sections.size() );
        for ( const auto& section : sections )
        {
            READ_SECTION( trig )
            READ_SECTION( source )
            READ_SECTION( filter )
            READ_SECTION( amp )
            READ_SECTION( lfo )
            READ_SECTION( delay )
            READ_SECTION( reverb )
        }

        return device_spec;
    }
#undef READ_SECTION
} // namespace sequencer::backend::digitakt
