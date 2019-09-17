#pragma once

#include <sequencer/midi/device_spec.hpp>
#include <sequencer/midi/message/message_type.hpp>
#include <sequencer/midi/pattern.hpp>
#include <sequencer/midi/track.hpp>
#include <sequencer/rtmidi/util.hpp>

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

namespace sequencer::backend
{
    enum class digitakt_mode
    {
        play,
        mute,
        bank_select,
        pattern_select,
        track_select,
        step_select
    };

    enum class digitakt_control_mode
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

    inline midi::device_spec_cc_t get_spec( const std::string& spec_filename )
    {
        auto spec_file = std::ifstream( spec_filename );
        if ( !spec_file.is_open() )
        {
            std::cerr << "Error: Could not open device specification file.\n";
            return {};
        }
        return midi::read_device_spec_cc( spec_file );
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

    class digitakt
    {
    public:
        using pattern_t = midi::pattern_t< midi::sequencer_track_t >;
        using patterns_t = std::vector< pattern_t >;
        using banks_t = std::vector< patterns_t >;

        explicit digitakt( const midi::device_spec_cc_t& device_spec = midi::device_spec_cc_t{} )
            : banks_( 16,
                      patterns_t( 16, midi::make_pattern< midi::sequencer_track_t >( 16, 16 ) ) ),
              device_spec_{device_spec}
        {
        }

        void set_step( int idx, bool value = true ) noexcept
        {
            switch ( mode_ )
            {
            case digitakt_mode::mute:
                current_pattern()[ idx ].mute( !current_pattern()[ idx ].is_muted() );
                return;
            case digitakt_mode::track_select:
                current_track_idx_ = decltype( current_track_idx_ )( idx );
                set_mode( digitakt_mode::play );
                return;
            case digitakt_mode::pattern_select:
                current_pattern_idx_ = decltype( current_pattern_idx_ )( idx );
                set_mode( digitakt_mode::play );
                return;
            case digitakt_mode::bank_select:
                current_bank_idx_ = decltype( current_bank_idx_ )( idx );
                set_mode( digitakt_mode::pattern_select );
                return;
            case digitakt_mode::play:
            {
                const auto step = value ? midi::step_t{value} : midi::step_t{};
                current_track()[ midi::sequencer_track_t::size_type( idx ) ] = step;
                return;
            }
            case digitakt_mode::step_select:
                if ( current_step_ == idx ||
                     !current_track()[ midi::sequencer_track_t::size_type( idx ) ].is_active() )
                {
                    set_mode( digitakt_mode::step_select );
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
            case digitakt_control_mode::trig:
                process_trig( id, value, sender );
                return;

            case digitakt_control_mode::source:
                current_track().source_parameter[ id ] = value;
                adjust_value_for_midi();
                sender( midi::channel::voice::control_change(
                    current_track().channel(), device_spec_.source[ id ].cc_key, value ) );
                return;

            case digitakt_control_mode::filter:
                current_track().filter_parameter[ id ] = value;
                adjust_value_for_midi();
                sender( midi::channel::voice::control_change(
                    current_track().channel(), device_spec_.filter[ id ].cc_key, value ) );
                return;

            case digitakt_control_mode::amp:
                current_track().amp_parameter[ id ] = value;
                adjust_value_for_midi();
                sender( midi::channel::voice::control_change(
                    current_track().channel(), device_spec_.amp[ id ].cc_key, value ) );
                return;

            case digitakt_control_mode::lfo:
                current_track().lfo_parameter[ id ] = value;
                adjust_value_for_midi();
                if ( device_spec_.lfo[ id ].cc_key_lsb > 0 )
                {
                    const auto [ msb, lsb ] =
                        to_msb_lsb( value, spec()[ id ].min, spec()[ id ].max );
                    sender( midi::channel::voice::control_change(
                        current_track().channel(), device_spec_.lfo[ id ].cc_key, msb ) );
                    sender( midi::channel::voice::control_change(
                        current_track().channel(), device_spec_.lfo[ id ].cc_key_lsb, lsb ) );
                    return;
                }
                sender( midi::channel::voice::control_change(
                    current_track().channel(), device_spec_.lfo[ id ].cc_key, value ) );
                return;

            case digitakt_control_mode::delay:
                current_track().delay_parameter[ id ] = value;
                adjust_value_for_midi();
                sender( midi::channel::voice::control_change(
                    current_track().channel(), device_spec_.delay[ id ].cc_key, value ) );
                return;

            case digitakt_control_mode::reverb:
                current_track().reverb_parameter[ id ] = value;
                adjust_value_for_midi();
                sender( midi::channel::voice::control_change(
                    current_track().channel(), device_spec_.reverb[ id ].cc_key, value ) );
                return;
            }
        }

        bool is_step_set( int step ) const noexcept
        {
            if ( mode_ == digitakt_mode::mute )
            {
                return current_pattern()[ step ].is_muted();
            }

            return current_track()[ midi::sequencer_track_t::size_type( step ) ].is_active();
        }

        constexpr void set_mode( digitakt_mode mode ) noexcept
        {
            if ( mode_ == mode && mode_ == digitakt_mode::mute )
            {
                mode_ = digitakt_mode::play;
                return;
            }
            if ( mode_ == mode && mode_ == digitakt_mode::step_select )
            {
                mode_ = digitakt_mode::play;
                return;
            }
            mode_ = mode;
            if ( mode_ != digitakt_mode::step_select )
            {
                current_step_ = -1;
            }
        }

        constexpr digitakt_mode mode() const noexcept
        {
            return mode_;
        }

        constexpr void set_control_mode( digitakt_control_mode mode ) noexcept
        {
            control_mode_ = mode;
        }

        constexpr digitakt_control_mode control_mode() const noexcept
        {
            return control_mode_;
        }

        template < class Sender >
        void receive_clock_message( sequencer::midi::message_t< 1 > message, Sender sender )
        {
            current_pattern().send_messages( message, sender );
        }

        sequencer::midi::sequencer_track_t& current_track() noexcept
        {
            return current_pattern()[ current_track_idx_ ];
        }

        const sequencer::midi::sequencer_track_t& current_track() const noexcept
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

        const std::array< midi::device_entry_t, 8 >& spec() const
        {
            return control_mode() == digitakt_control_mode::trig
                       ? device_spec_.trig
                       : control_mode() == digitakt_control_mode::source
                             ? device_spec_.source
                             : control_mode() == digitakt_control_mode::filter
                                   ? device_spec_.filter
                                   : control_mode() == digitakt_control_mode::amp
                                         ? device_spec_.amp
                                         : control_mode() == digitakt_control_mode::lfo
                                               ? device_spec_.lfo
                                               : control_mode() == digitakt_control_mode::delay
                                                     ? device_spec_.delay
                                                     : device_spec_.reverb;
        }

    private:
        constexpr std::pair< std::uint8_t, std::uint8_t > to_msb_lsb( int value, int min,
                                                                      int max ) const noexcept
        {
            const auto spread = max - min;
            const auto a = ( spread % midi::number_of_values_per_byte )
                               ? 1 + spread / midi::number_of_values_per_byte
                               : spread / midi::number_of_values_per_byte;
            const auto msb = value / a;
            const auto lsb =
                int( ( ( value % a ) / double( a ) ) * midi::number_of_values_per_byte );
            return {msb, lsb};
        }

        template < class Sender >
        void process_trig( int id, double value, Sender )
        {
            if ( mode() == digitakt_mode::step_select )
            {
                switch ( id )
                {
                case 0:
                    current_track()[ current_step_ ].set_note(
                        sequencer::midi::note_t( to_uint8_t( current_track().base_note() ) +
                                                 std::uint8_t( value * 1.001 ) ) );
                    return;
                case 1:
                    assert( current_step_ != -1 );
                    current_track()[ current_step_ ].set_velocity( std::uint8_t( value * 1.001 ) );
                }
                return;
            }
            switch ( id )
            {
            case 0:
                current_track().set_note_offset( value * 1.001 );
                return;
            case 1:
                current_track().set_velocity( std::uint8_t( value * 1.001 ) );
            }
        }

        banks_t banks_;
        banks_t::size_type current_bank_idx_ = 0;
        patterns_t::size_type current_pattern_idx_ = 0;
        pattern_t::size_type current_track_idx_ = 0;
        digitakt_mode mode_ = digitakt_mode::play;
        digitakt_control_mode control_mode_ = digitakt_control_mode::trig;
        int current_step_{-1};
        sequencer::midi::device_spec_cc_t device_spec_;
    };
} // namespace sequencer::backend
