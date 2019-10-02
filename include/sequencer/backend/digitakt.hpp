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

    template < class Sender, class Receiver >
    struct clock_comm_t
    {
        Sender clock_sender;
        Receiver clock_receiver;
    };

    template < class Clock, class Backend, class ClockSender, class ClockReceiver, class Sender >
    class backend_t : public clock_comm_t< ClockSender, ClockReceiver >,
                      public Sender,
                      public Backend,
                      public Clock
    {
    public:
        template < class Callback >
        explicit backend_t( Callback callback )
            : clock_comm_t< ClockSender, ClockReceiver >{ClockSender{}, ClockReceiver{callback}},
              Sender{}, Backend{get_spec( "device_spec/elektron/digitakt.txt" )},
              Clock( [this]( midi::message_t< 1 > message ) {
                  clock_comm_t< ClockSender, ClockReceiver >::clock_sender.sender()( message );
              } )
        {
        }

        void set_control( int id, int value ) noexcept
        {
            Backend::set_control( id, value, Sender::sender() );
        }
    };

#define ADD_LFO_DESTINATION( section_name )                                                        \
    {                                                                                              \
        auto& entry = device_spec_.control_parameter[ track_parameter_t::idx::section_name ];      \
        for ( decltype( entry.size() ) i = 0; i < entry.size(); ++i )                              \
        {                                                                                          \
            device_spec_                                                                           \
                .control_parameter[ track_parameter_t::idx::lfo ]                                  \
                                  [ track_parameter_t::lfo_idx::destination ]                      \
                .str_map.push_back( #section_name + std::string( ":" ) + entry[ i ].name );        \
            lfo_map_.emplace_back( control_mode_t::section_name, i );                              \
        }                                                                                          \
    }

    inline midi::trig_condition_t get_trig_condition( int value )
    {
        if ( value == 0 )
        {
            return {};
        }
        switch ( value - 1 )
        {
        // fill
        case 0:
        // not fill
        case 1:
        // pre
        case 2:
        // not pre
        case 3:
        // neighbor
        case 4:
        // not neighbor
        case 5:
        // first
        case 6:
        // not first
        case 7:
            return {};
        // percentage
        case 8:
            return midi::trig_condition::random{1};

        case 9:
            return midi::trig_condition::random{2};

        case 10:
            return midi::trig_condition::random{4};

        case 11:
            return midi::trig_condition::random{6};

        case 12:
            return midi::trig_condition::random{9};

        case 13:
            return midi::trig_condition::random{13};

        case 14:
            return midi::trig_condition::random{19};

        case 15:
            return midi::trig_condition::random{25};

        case 16:
            return midi::trig_condition::random{33};

        case 17:
            return midi::trig_condition::random{41};

        case 18:
            return midi::trig_condition::random{50};

        case 19:
            return midi::trig_condition::random{59};

        case 20:
            return midi::trig_condition::random{67};

        case 21:
            return midi::trig_condition::random{75};

        case 22:
            return midi::trig_condition::random{81};

        case 23:
            return midi::trig_condition::random{87};

        case 24:
            return midi::trig_condition::random{91};

        case 25:
            return midi::trig_condition::random{94};

        case 26:
            return midi::trig_condition::random{96};

        case 27:
            return midi::trig_condition::random{98};

        case 28:
            return midi::trig_condition::random{99};

        case 29:
            return midi::trig_condition::deterministic< 1, 2 >{};

        case 30:
            return midi::trig_condition::deterministic< 2, 2 >{};

        case 31:
            return midi::trig_condition::deterministic< 1, 3 >{};

        case 32:
            return midi::trig_condition::deterministic< 2, 3 >{};

        case 33:
            return midi::trig_condition::deterministic< 3, 3 >{};

        case 34:
            return midi::trig_condition::deterministic< 1, 4 >{};

        case 35:
            return midi::trig_condition::deterministic< 2, 4 >{};

        case 36:
            return midi::trig_condition::deterministic< 3, 4 >{};

        case 37:
            return midi::trig_condition::deterministic< 4, 4 >{};

        case 38:
            return midi::trig_condition::deterministic< 1, 5 >{};

        case 39:
            return midi::trig_condition::deterministic< 2, 5 >{};

        case 40:
            return midi::trig_condition::deterministic< 3, 5 >{};

        case 41:
            return midi::trig_condition::deterministic< 4, 5 >{};

        case 42:
            return midi::trig_condition::deterministic< 5, 5 >{};

        case 43:
            return midi::trig_condition::deterministic< 1, 6 >{};

        case 44:
            return midi::trig_condition::deterministic< 2, 6 >{};

        case 45:
            return midi::trig_condition::deterministic< 3, 6 >{};

        case 46:
            return midi::trig_condition::deterministic< 4, 6 >{};

        case 47:
            return midi::trig_condition::deterministic< 5, 6 >{};

        case 48:
            return midi::trig_condition::deterministic< 6, 6 >{};

        case 49:
            return midi::trig_condition::deterministic< 1, 7 >{};

        case 50:
            return midi::trig_condition::deterministic< 2, 7 >{};

        case 51:
            return midi::trig_condition::deterministic< 3, 7 >{};

        case 52:
            return midi::trig_condition::deterministic< 4, 7 >{};

        case 53:
            return midi::trig_condition::deterministic< 5, 7 >{};

        case 54:
            return midi::trig_condition::deterministic< 6, 7 >{};

        case 55:
            return midi::trig_condition::deterministic< 7, 7 >{};

        case 56:
            return midi::trig_condition::deterministic< 1, 8 >{};

        case 57:
            return midi::trig_condition::deterministic< 2, 8 >{};

        case 58:
            return midi::trig_condition::deterministic< 3, 8 >{};

        case 59:
            return midi::trig_condition::deterministic< 4, 8 >{};

        case 60:
            return midi::trig_condition::deterministic< 5, 8 >{};

        case 61:
            return midi::trig_condition::deterministic< 6, 8 >{};

        case 62:
            return midi::trig_condition::deterministic< 7, 8 >{};

        case 63:
            return midi::trig_condition::deterministic< 8, 8 >{};
        }

        return {};
    }

    class rtmidi_receiver_t
    {
    public:
        template < class Callback >
        explicit rtmidi_receiver_t( Callback callback )
        {
            midiin_.setCallback( callback );
            // Don't ignore sysex, timing, or active sensing messages.
            midiin_.ignoreTypes( false, false, false );
        }

        void select_input_port( int idx )
        {
            assert( idx >= 0 );
            assert( idx <= int( midiin_.getPortCount() ) + 1 );

            if ( midiin_.isPortOpen() )
            {
                midiin_.closePort();
            }
            if ( idx == 0 )
            {
                return;
            }
            midiin_.openPort( unsigned( idx - 1 ) );
        }

        std::vector< std::string > available_input_ports()
        {
            std::vector< std::string > ports;
            for ( auto id = 0u; id < midiin_.getPortCount(); ++id )
            {
                ports.push_back( midiin_.getPortName( id ) );
            }
            return ports;
        }

    private:
        RtMidiIn midiin_{};
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
                        for ( decltype( track.parameter().size() ) i = 0;
                              i < track.parameter().size(); ++i )
                        {
                            track.parameter()[ i ].resize(
                                device_spec.control_parameter[ i ].size() );
                            for ( decltype( track.parameter()[ i ].size() ) j = 0;
                                  j < track.parameter()[ i ].size(); ++j )
                            {
                                const auto value = device_spec.control_parameter[ i ][ j ].value;
                                track.parameter()[ i ][ j ] =
                                    device_spec.control_parameter[ i ][ j ].map.empty()
                                        ? value
                                        : device_spec.control_parameter[ i ][ j ].map[ value ];
                            }
                        }
                    }
                }
            }

            device_spec_
                .control_parameter[ track_parameter_t::idx::lfo ]
                                  [ track_parameter_t::lfo_idx::destination ]
                .str_map.emplace_back( "none" );
            ADD_LFO_DESTINATION( trig )
            ADD_LFO_DESTINATION( source )
            ADD_LFO_DESTINATION( filter )
            ADD_LFO_DESTINATION( amp )
            ADD_LFO_DESTINATION( delay )
            ADD_LFO_DESTINATION( reverb )
            device_spec_
                .control_parameter[ track_parameter_t::idx::lfo ]
                                  [ track_parameter_t::lfo_idx::destination ]
                .max =
                std::int16_t( device_spec_
                                  .control_parameter[ track_parameter_t::idx::lfo ]
                                                    [ track_parameter_t::lfo_idx::destination ]
                                  .str_map.size() -
                              1 );
        }
#undef ADD_LFO_DESTINATION

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
            namespace trig_condition = sequencer::midi::trig_condition;

            const auto adjust_value_for_midi = [id, &value, this] {
                value -= spec()[ std::size_t( id ) ].min;
            };

            switch ( control_mode() )
            {
            case control_mode_t::trig:
            {
                if ( mode() == mode_t::step_select )
                {
                    switch ( id )
                    {
                    case 0:
                        assert( current_step_ != -1 );
                        current_track()[ current_step_ ].set_note( sequencer::midi::note_t(
                            to_uint8_t( current_track().base_note() ) + std::uint8_t( value ) ) );
                        return;
                    case 1:
                        assert( current_step_ != -1 );
                        current_track()[ current_step_ ].set_velocity( std::uint8_t( value ) );
                        return;
                    case 3:
                        assert( current_step_ != -1 );
                        current_track()[ current_step() ].set_trig_condition(
                            get_trig_condition( value ) );
                        return;
                    }
                    return;
                }

                current_track().parameter()[ track_parameter_t::idx::trig ][ std::size_t( id ) ] =
                    value;
                adjust_value_for_midi();
                sender( midi::channel::voice::control_change(
                    current_track().channel(),
                    device_spec_
                        .control_parameter[ track_parameter_t::idx::trig ][ std::size_t( id ) ]
                        .cc_msb,
                    value ) );
            }
                return;

            case control_mode_t::source:
                current_track().parameter()[ track_parameter_t::idx::source ][ std::size_t( id ) ] =
                    value;
                adjust_value_for_midi();
                sender( midi::channel::voice::control_change(
                    current_track().channel(),
                    device_spec_
                        .control_parameter[ track_parameter_t::idx::source ][ std::size_t( id ) ]
                        .cc_msb,
                    value ) );
                return;

            case control_mode_t::filter:
                current_track().parameter()[ track_parameter_t::idx::filter ][ std::size_t( id ) ] =
                    value;
                adjust_value_for_midi();
                sender( midi::channel::voice::control_change(
                    current_track().channel(),
                    device_spec_
                        .control_parameter[ track_parameter_t::idx::filter ][ std::size_t( id ) ]
                        .cc_msb,
                    value ) );
                return;

            case control_mode_t::amp:
                current_track().parameter()[ track_parameter_t::idx::amp ][ std::size_t( id ) ] =
                    value;
                adjust_value_for_midi();
                sender( midi::channel::voice::control_change(
                    current_track().channel(),
                    device_spec_
                        .control_parameter[ track_parameter_t::idx::amp ][ std::size_t( id ) ]
                        .cc_msb,
                    value ) );
                return;

            case control_mode_t::lfo:
            {
                if ( spec()[ std::size_t( id ) ].map.empty() )
                {
                    current_track()
                        .parameter()[ track_parameter_t::idx::lfo ][ std::size_t( id ) ] = value;
                }
                else
                {
                    current_track()
                        .parameter()[ track_parameter_t::idx::lfo ][ std::size_t( id ) ] =
                        spec()[ std::size_t( id ) ].map[ value ];
                }
                const auto dest = current_track()
                                      .parameter()[ track_parameter_t::idx::lfo ]
                                                  [ track_parameter_t::lfo_idx::destination ]
                                      .load();
                if ( dest > 0 )
                {
                    const auto target = lfo_map_[ std::size_t( dest - 1 ) ];
                    const auto& entry = spec( target.first )[ target.second ];
                    current_track().set_lfo(
                        entry.min, entry.max,
                        [cc_msb = entry.cc_msb,
                         channel = current_track().channel()]( std::uint8_t value ) {
                            return midi::channel::voice::control_change( channel, cc_msb, value );
                        } );
                }
                adjust_value_for_midi();
                if ( device_spec_
                         .control_parameter[ track_parameter_t::idx::lfo ][ std::size_t( id ) ]
                         .cc_lsb > 0 )
                {
                    const auto [ msb, lsb ] = to_msb_lsb( value, spec()[ std::size_t( id ) ].min,
                                                          spec()[ std::size_t( id ) ].max );
                    sender( midi::channel::voice::control_change(
                        current_track().channel(),
                        device_spec_
                            .control_parameter[ track_parameter_t::idx::lfo ][ std::size_t( id ) ]
                            .cc_msb,
                        msb ) );
                    sender( midi::channel::voice::control_change(
                        current_track().channel(),
                        device_spec_
                            .control_parameter[ track_parameter_t::idx::lfo ][ std::size_t( id ) ]
                            .cc_lsb,
                        lsb ) );
                    return;
                }
                sender( midi::channel::voice::control_change(
                    current_track().channel(),
                    device_spec_
                        .control_parameter[ track_parameter_t::idx::lfo ][ std::size_t( id ) ]
                        .cc_msb,
                    value ) );
                return;
            }

            case control_mode_t::delay:
                current_track().parameter()[ track_parameter_t::idx::delay ][ std::size_t( id ) ] =
                    value;
                adjust_value_for_midi();
                sender( midi::channel::voice::control_change(
                    current_track().channel(),
                    device_spec_
                        .control_parameter[ track_parameter_t::idx::delay ][ std::size_t( id ) ]
                        .cc_msb,
                    value ) );
                return;

            case control_mode_t::reverb:
                current_track().parameter()[ track_parameter_t::idx::reverb ][ std::size_t( id ) ] =
                    value;
                adjust_value_for_midi();
                sender( midi::channel::voice::control_change(
                    current_track().channel(),
                    device_spec_
                        .control_parameter[ track_parameter_t::idx::reverb ][ std::size_t( id ) ]
                        .cc_msb,
                    value ) );
                return;
            }
        }

        int get_control_value( int id ) const noexcept
        {
            switch ( control_mode() )
            {
            case control_mode_t::trig:
                return current_track()
                    .parameter()[ track_parameter_t::idx::trig ][ std::size_t( id ) ]
                    .load();
            case control_mode_t::source:
                return current_track()
                    .parameter()[ track_parameter_t::idx::source ][ std::size_t( id ) ]
                    .load();
            case control_mode_t::filter:
                return current_track()
                    .parameter()[ track_parameter_t::idx::filter ][ std::size_t( id ) ]
                    .load();
            case control_mode_t::amp:
                return current_track()
                    .parameter()[ track_parameter_t::idx::amp ][ std::size_t( id ) ]
                    .load();
            case control_mode_t::lfo:

                if ( spec()[ std::size_t( id ) ].map.empty() )
                {
                    return current_track()
                        .parameter()[ track_parameter_t::idx::lfo ][ std::size_t( id ) ]
                        .load();
                }
                return int( std::distance(
                    begin( spec()[ std::size_t( id ) ].map ),
                    std::find( begin( spec()[ std::size_t( id ) ].map ),
                               end( spec()[ std::size_t( id ) ].map ),
                               current_track()
                                   .parameter()[ track_parameter_t::idx::lfo ][ std::size_t( id ) ]
                                   .load() ) ) );
            case control_mode_t::delay:
                return current_track()
                    .parameter()[ track_parameter_t::idx::delay ][ std::size_t( id ) ]
                    .load();
            case control_mode_t::reverb:
                return current_track()
                    .parameter()[ track_parameter_t::idx::reverb ][ std::size_t( id ) ]
                    .load();
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
            std::cout << "receive clock message" << std::endl;
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

        const std::vector< midi::device_entry_t >& spec( control_mode_t mode ) const
        {
            return mode == control_mode_t::trig
                       ? device_spec_.control_parameter[ track_parameter_t::idx::trig ]
                       : mode == control_mode_t::source
                             ? device_spec_.control_parameter[ track_parameter_t::idx::source ]
                             : mode == control_mode_t::filter
                                   ? device_spec_
                                         .control_parameter[ track_parameter_t::idx::filter ]
                                   : mode == control_mode_t::amp
                                         ? device_spec_
                                               .control_parameter[ track_parameter_t::idx::amp ]
                                         : mode == control_mode_t::lfo
                                               ? device_spec_.control_parameter
                                                     [ track_parameter_t::idx::lfo ]
                                               : mode == control_mode_t::delay
                                                     ? device_spec_.control_parameter
                                                           [ track_parameter_t::idx::delay ]
                                                     : device_spec_.control_parameter
                                                           [ track_parameter_t::idx::reverb ];
        }

        const std::vector< midi::device_entry_t >& spec() const
        {
            return spec( control_mode() );
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
        std::vector< std::pair< control_mode_t, std::size_t > > lfo_map_;
    };
} // namespace sequencer::backend::digitakt
