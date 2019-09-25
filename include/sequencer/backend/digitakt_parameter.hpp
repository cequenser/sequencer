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
                length_map[ i ] = value;
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

        track_parameter_t() : midi::track_parameter_t{idx::count, length_idx + 1}
        {
        }

        std::uint16_t note_offset() const noexcept
        {
            return values[ idx::trig ][ note_offset_idx ].load();
        }

        std::uint16_t velocity() const noexcept
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

        std::uint16_t lfo_destination() const noexcept
        {
            return values[ idx::lfo ][ lfo_idx::destination ].load();
        }

        std::uint16_t lfo_speed() const noexcept
        {
            return values[ idx::lfo ][ lfo_idx::speed ].load();
        }

        bool lfo_enabled() const noexcept
        {
            return lfo_destination() > 0 && lfo_speed() > 0;
        }

        auto lfo_func( const midi::device_entry_t& entry ) const noexcept
        {
            return [this, &entry]( std::size_t pulse_count, std::size_t steps_per_quarter_note,
                                   std::size_t pulses_per_quarter_note ) {
                const auto speed = lfo_speed() * 2; // values[idx::lfo][lfo_idx::multiplier].load();
                const auto phase = values[ idx::lfo ][ lfo_idx::phase ].load();
                const auto lfo_value = midi::lfo< std::uint8_t >(
                    pulse_count, steps_per_quarter_note, pulses_per_quarter_note, speed, phase,
                    entry.min, entry.max, midi::lfo_mode::square );
                return midi::channel::voice::control_change( 0, entry.cc_msb, lfo_value );
            };
        }
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
