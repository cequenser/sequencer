#pragma once

#include <sequencer/backend/digitakt_parameter.hpp>
#include <sequencer/midi/device_spec.hpp>
#include <sequencer/midi/message/message_type.hpp>
#include <sequencer/midi/pattern.hpp>
#include <sequencer/midi/track.hpp>
#include <sequencer/rtmidi/util.hpp>

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

namespace sequencer::backend::digitakt
{
    enum class mode_t
    {
        play,
        mute,
        bank_select,
        pattern_select,
        track_select,
        step_select
    };

    enum class control_mode_t
    {
        trig,
        quantize,
        source,
        assign,
        filter,
        delay,
        amp,
        reverb,
        lfo,
        master
    };

    inline device_spec_cc_t get_spec( const std::string& spec_filename )
    {
        auto spec_file = std::ifstream( spec_filename );
        if ( !spec_file.is_open() )
        {
            std::cerr << "Error: Could not open device specification file.\n";
            return {};
        }
        return read_device_spec_cc( spec_file );
    }

    template < class Clock, class Backend, class Sender >
    class backend_t : public Sender, public Backend, public Clock
    {
    public:
        backend_t()
            : Sender{}, Backend{get_spec( "device_spec/elektron/digitakt.txt" )},
              Clock( [this]( midi::message_t< 1 > message ) {
                  using rtmidi::message_sender;
                  Sender::sender()( message );
                  Backend::receive_clock_message( message, Sender::sender() );
              } )
        {
        }

        void set_control( int id, int value ) noexcept
        {
            Backend::set_control( id, value, Sender::sender() );
        }
    };

    class backend_impl
    {
    public:
        using track_t = midi::track_t< track_parameter_t >;
        using pattern_t = midi::pattern_t< track_t >;
        using patterns_t = std::vector< pattern_t >;
        using banks_t = std::vector< patterns_t >;

        explicit backend_impl( const device_spec_cc_t& device_spec = device_spec_cc_t{} )
            : banks_( 16, patterns_t( 16, midi::make_pattern< track_t >( 16, 16 ) ) ),
              device_spec_{device_spec}
        {
            for ( auto& patterns : banks_ )
            {
                for ( auto& pattern : patterns )
                {
                    for ( auto& track : pattern )
                    {
                        track.parameter().resize( device_spec.control_parameter.size() );
                        for ( auto i = 0u; i < track.parameter().size(); ++i )
                        {
                            track.parameter()[ i ].resize(
                                device_spec.control_parameter[ i ].size() );
                            for ( auto j = 0u; j < track.parameter()[ i ].size(); ++j )
                            {
                                track.parameter()[ i ][ j ] =
                                    device_spec.control_parameter[ i ][ j ].value;
                            }
                        }
                    }
                }
            }
        }

        void set_step( int idx, bool value = true ) noexcept
        {
            switch ( mode_ )
            {
            case mode_t::mute:
                current_pattern()[ idx ].mute( !current_pattern()[ idx ].is_muted() );
                return;
            case mode_t::track_select:
                current_track_idx_ = decltype( current_track_idx_ )( idx );
                set_mode( mode_t::play );
                return;
            case mode_t::pattern_select:
                current_pattern_idx_ = decltype( current_pattern_idx_ )( idx );
                set_mode( mode_t::play );
                return;
            case mode_t::bank_select:
                current_bank_idx_ = decltype( current_bank_idx_ )( idx );
                set_mode( mode_t::pattern_select );
                return;
            case mode_t::play:
            {
                const auto step = value ? midi::step_t{value} : midi::step_t{};
                current_track()[ track_t::size_type( idx ) ] = step;
                return;
            }
            case mode_t::step_select:
                if ( current_step_ == idx ||
                     !current_track()[ track_t::size_type( idx ) ].is_active() )
                {
                    set_mode( mode_t::step_select );
                    current_step_ = -1;
                    return;
                }
                current_step_ = idx;
                return;
            }
        }

        template < class Sender >
        void set_control( int id, int value, Sender sender ) noexcept
        {
            const auto adjust_value_for_midi = [id, &value, this] { value -= spec()[ id ].min; };

            switch ( control_mode() )
            {
            case control_mode_t::trig:
            {
                if ( mode() == mode_t::step_select )
                {
                    switch ( id )
                    {
                    case 0:
                        current_track()[ current_step_ ].set_note( sequencer::midi::note_t(
                            to_uint8_t( current_track().base_note() ) + std::uint8_t( value ) ) );
                        return;
                    case 1:
                        assert( current_step_ != -1 );
                        current_track()[ current_step_ ].set_velocity( std::uint8_t( value ) );
                    }
                    return;
                }

                current_track().parameter()[ track_parameter_t::idx::trig ][ id ] = value;
                adjust_value_for_midi();
                sender( midi::channel::voice::control_change(
                    current_track().channel(),
                    device_spec_.control_parameter[ track_parameter_t::idx::trig ][ id ].cc_msb,
                    value ) );
            }
                return;

            case control_mode_t::source:
                current_track().parameter()[ track_parameter_t::idx::source ][ id ] = value;
                adjust_value_for_midi();
                sender( midi::channel::voice::control_change(
                    current_track().channel(),
                    device_spec_.control_parameter[ track_parameter_t::idx::source ][ id ].cc_msb,
                    value ) );
                return;

            case control_mode_t::filter:
                current_track().parameter()[ track_parameter_t::idx::filter ][ id ] = value;
                adjust_value_for_midi();
                sender( midi::channel::voice::control_change(
                    current_track().channel(),
                    device_spec_.control_parameter[ track_parameter_t::idx::filter ][ id ].cc_msb,
                    value ) );
                return;

            case control_mode_t::amp:
                current_track().parameter()[ track_parameter_t::idx::amp ][ id ] = value;
                adjust_value_for_midi();
                sender( midi::channel::voice::control_change(
                    current_track().channel(),
                    device_spec_.control_parameter[ track_parameter_t::idx::amp ][ id ].cc_msb,
                    value ) );
                return;

            case control_mode_t::lfo:
                current_track().parameter()[ track_parameter_t::idx::lfo ][ id ] = value;
                adjust_value_for_midi();
                if ( device_spec_.control_parameter[ track_parameter_t::idx::lfo ][ id ].cc_lsb >
                     0 )
                {
                    const auto [ msb, lsb ] =
                        to_msb_lsb( value, spec()[ id ].min, spec()[ id ].max );
                    sender( midi::channel::voice::control_change(
                        current_track().channel(),
                        device_spec_.control_parameter[ track_parameter_t::idx::lfo ][ id ].cc_msb,
                        msb ) );
                    sender( midi::channel::voice::control_change(
                        current_track().channel(),
                        device_spec_.control_parameter[ track_parameter_t::idx::lfo ][ id ].cc_lsb,
                        lsb ) );
                    return;
                }
                sender( midi::channel::voice::control_change(
                    current_track().channel(),
                    device_spec_.control_parameter[ track_parameter_t::idx::lfo ][ id ].cc_msb,
                    value ) );
                return;

            case control_mode_t::delay:
                current_track().parameter()[ track_parameter_t::idx::delay ][ id ] = value;
                adjust_value_for_midi();
                sender( midi::channel::voice::control_change(
                    current_track().channel(),
                    device_spec_.control_parameter[ track_parameter_t::idx::delay ][ id ].cc_msb,
                    value ) );
                return;

            case control_mode_t::reverb:
                current_track().parameter()[ track_parameter_t::idx::reverb ][ id ] = value;
                adjust_value_for_midi();
                sender( midi::channel::voice::control_change(
                    current_track().channel(),
                    device_spec_.control_parameter[ track_parameter_t::idx::reverb ][ id ].cc_msb,
                    value ) );
                return;
            }
        }

        int get_control_value( int id ) const noexcept
        {
            switch ( control_mode() )
            {
            case control_mode_t::trig:
                return current_track().parameter()[ track_parameter_t::idx::trig ][ id ].load();
            case control_mode_t::source:
                return current_track().parameter()[ track_parameter_t::idx::source ][ id ].load();
            case control_mode_t::filter:
                return current_track().parameter()[ track_parameter_t::idx::filter ][ id ].load();
            case control_mode_t::amp:
                return current_track().parameter()[ track_parameter_t::idx::amp ][ id ].load();
            case control_mode_t::lfo:
                return current_track().parameter()[ track_parameter_t::idx::lfo ][ id ].load();
            case control_mode_t::delay:
                return current_track().parameter()[ track_parameter_t::idx::delay ][ id ].load();
            case control_mode_t::reverb:
                return current_track().parameter()[ track_parameter_t::idx::reverb ][ id ].load();
            default:
                return 0;
            }
        }

        bool is_step_set( int step ) const noexcept
        {
            if ( mode_ == mode_t::mute )
            {
                return current_pattern()[ step ].is_muted();
            }

            return current_track()[ track_t::size_type( step ) ].is_active();
        }

        constexpr void set_mode( mode_t mode ) noexcept
        {
            if ( mode_ == mode && mode_ == mode_t::mute )
            {
                mode_ = mode_t::play;
                return;
            }
            if ( mode_ == mode && mode_ == mode_t::step_select )
            {
                mode_ = mode_t::play;
                return;
            }
            mode_ = mode;
            if ( mode_ != mode_t::step_select )
            {
                current_step_ = -1;
            }
        }

        constexpr mode_t mode() const noexcept
        {
            return mode_;
        }

        constexpr void set_control_mode( control_mode_t mode ) noexcept
        {
            control_mode_ = mode;
        }

        constexpr control_mode_t control_mode() const noexcept
        {
            return control_mode_;
        }

        template < class Sender >
        void receive_clock_message( sequencer::midi::message_t< 1 > message, Sender sender )
        {
            current_pattern().send_messages( message, sender );
        }

        track_t& current_track() noexcept
        {
            return current_pattern()[ current_track_idx_ ];
        }

        const track_t& current_track() const noexcept
        {
            return current_pattern()[ current_track_idx_ ];
        }

        pattern_t& current_pattern() noexcept
        {
            return banks_[ current_bank_idx_ ][ current_pattern_idx_ ];
        }

        const pattern_t& current_pattern() const noexcept
        {
            return banks_[ current_bank_idx_ ][ current_pattern_idx_ ];
        }

        constexpr int current_step() const noexcept
        {
            return current_step_;
        }

        const std::vector< midi::device_entry_t >& spec() const
        {
            return control_mode() == control_mode_t::trig
                       ? device_spec_.control_parameter[ track_parameter_t::idx::trig ]
                       : control_mode() == control_mode_t::source
                             ? device_spec_.control_parameter[ track_parameter_t::idx::source ]
                             : control_mode() == control_mode_t::filter
                                   ? device_spec_
                                         .control_parameter[ track_parameter_t::idx::filter ]
                                   : control_mode() == control_mode_t::amp
                                         ? device_spec_
                                               .control_parameter[ track_parameter_t::idx::amp ]
                                         : control_mode() == control_mode_t::lfo
                                               ? device_spec_.control_parameter
                                                     [ track_parameter_t::idx::lfo ]
                                               : control_mode() == control_mode_t::delay
                                                     ? device_spec_.control_parameter
                                                           [ track_parameter_t::idx::delay ]
                                                     : device_spec_.control_parameter
                                                           [ track_parameter_t::idx::reverb ];
        }

    private:
        constexpr std::pair< std::uint8_t, std::uint8_t > to_msb_lsb( int value, int min,
                                                                      int max ) const noexcept
        {
            const auto spread = max - min;
            const auto a = ( spread % midi::number_of_values_per_byte > 0 )
                               ? 1 + spread / midi::number_of_values_per_byte
                               : spread / midi::number_of_values_per_byte;
            const auto msb = value / a;
            const auto lsb =
                int( ( ( value % a ) / double( a ) ) * midi::number_of_values_per_byte );
            return {msb, lsb};
        }

        banks_t banks_;
        banks_t::size_type current_bank_idx_ = 0;
        patterns_t::size_type current_pattern_idx_ = 0;
        pattern_t::size_type current_track_idx_ = 0;
        mode_t mode_ = mode_t::play;
        control_mode_t control_mode_ = control_mode_t::trig;
        int current_step_{-1};
        device_spec_cc_t device_spec_;
    };
} // namespace sequencer::backend::digitakt
