#pragma once

#include <sequencer/midi/pattern.hpp>
#include <sequencer/midi/track.hpp>

#include <vector>

namespace sequencer::backend
{
    enum class digitakt_mode
    {
        play,
        mute,
        bank_select,
        pattern_select,
        track_select
    };

    class digitakt
    {
    public:
        using pattern_t = midi::pattern_t< midi::sequencer_track_t >;
        using patterns_t = std::vector< pattern_t >;
        using banks_t = std::vector< patterns_t >;

        digitakt()
            : banks_( 16,
                      patterns_t( 16, midi::make_pattern< midi::sequencer_track_t >( 16, 16 ) ) )
        {
        }

        void set_step( int step, bool value = true ) noexcept
        {
            switch ( mode_ )
            {
            case digitakt_mode::mute:
                current_pattern()[ step ].mute( !current_pattern()[ step ].is_muted() );
                return;
            case digitakt_mode::track_select:
                current_track_idx_ = decltype( current_track_idx_ )( step );
                set_mode( digitakt_mode::play );
                return;
            case digitakt_mode::pattern_select:
                current_pattern_idx_ = decltype( current_pattern_idx_ )( step );
                set_mode( digitakt_mode::play );
                return;
            case digitakt_mode::bank_select:
                current_bank_idx_ = decltype( current_bank_idx_ )( step );
                set_mode( digitakt_mode::pattern_select );
                return;
            case digitakt_mode::play:
                const auto note = value ? midi::note_t{48} : midi::no_note();
                current_track()[ midi::sequencer_track_t::size_type( step ) ] = note;
            }
        }

        bool is_step_set( int step ) const noexcept
        {
            if ( mode_ == digitakt_mode::mute )
            {
                return current_pattern()[ step ].is_muted();
            }

            return current_track()[ midi::sequencer_track_t::size_type( step ) ] != midi::no_note();
        }

        constexpr void set_mode( digitakt_mode mode ) noexcept
        {
            if ( mode_ == mode && mode_ == digitakt_mode::mute )
            {
                mode_ = digitakt_mode::play;
                return;
            }
            mode_ = mode;
        }

        constexpr digitakt_mode mode() const noexcept
        {
            return mode_;
        }

        template < class Sender >
        void receive_clock_message( sequencer::midi::message_t< 1 > message, Sender sender )
        {
            send_messages( current_pattern(), message, sender );
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

    private:
        banks_t banks_;
        banks_t::size_type current_bank_idx_ = 0;
        patterns_t::size_type current_pattern_idx_ = 0;
        pattern_t::size_type current_track_idx_ = 0;
        digitakt_mode mode_ = digitakt_mode::play;
    };
} // namespace sequencer::backend
