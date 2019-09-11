#pragma once

#include <sequencer/midi/track.hpp>

#include <vector>

namespace sequencer::midi
{
    template < class Track >
    class pattern_t
    {
    public:
        using value_type = Track;
        using size_type = typename std::vector< value_type >::size_type;

        explicit pattern_t( size_type size, size_type track_size )
            : tracks_( size, Track( track_size ) )
        {
        }

        value_type& operator[]( size_type i )
        {
            return tracks_[ i ];
        }

        const value_type& operator[]( size_type i ) const
        {
            return tracks_[ i ];
        }

        auto begin()
        {
            return tracks_.begin();
        }

        auto begin() const
        {
            return tracks_.begin();
        }

        auto end()
        {
            return tracks_.end();
        }

        auto end() const
        {
            return tracks_.end();
        }

        size_type size() const noexcept
        {
            return tracks_.size();
        }

        void set_steps( std::size_t new_steps, std::size_t copy_offset = 0 )
        {
            for ( auto& track : tracks_ )
            {
                track.set_steps( new_steps, copy_offset );
            }
        }

        void set_steps_per_beat( std::size_t steps )
        {
            for ( auto& track : tracks_ )
            {
                track.set_steps_per_beat( steps );
            }
            clock_to_step_.set_steps_per_beat( steps );
        }

        void set_pulses_per_quarter_note( std::size_t pulses_per_quarter )
        {
            for ( auto& track : tracks_ )
            {
                track.set_pulses_per_quarter_note( pulses_per_quarter );
            }
            clock_to_step_.set_pulses_per_quarter_note( pulses_per_quarter );
        }

        constexpr void set_loop_length( std::size_t loop_length ) noexcept
        {
            clock_to_step_.set_steps( loop_length ? loop_length
                                                  : std::numeric_limits< std::size_t >::max() );
        }

        template < class Sender >
        void send_messages( message_t< 1 > message, const Sender& sender )
        {
            clock_to_step_.process_message( message );
            if ( clock_to_step_.midi_beat_counter() == 0 )
            {
                for ( auto& track : tracks_ )
                {
                    track.reset_beat_counter();
                }
            }
            for ( auto& track : tracks_ )
            {
                track.send_messages( message, sender );
            }
        }

    private:
        std::vector< value_type > tracks_;
        std::size_t loop_length_{16};
        clock_to_step_impl clock_to_step_;
    };

    template < class Track >
    pattern_t< Track >
    make_pattern( std::size_t number_of_tracks, std::size_t number_of_steps,
                  std::size_t pulses_per_quarter_note = default_pulses_per_quarter_note ) noexcept
    {
        auto pattern = pattern_t< Track >{number_of_tracks, number_of_steps};

        std::uint8_t channel = 0;
        for ( auto& track : pattern )
        {
            track.set_channel( channel++ );
            track.set_pulses_per_quarter_note( pulses_per_quarter_note );
        };

        return pattern;
    }

} // namespace sequencer::midi
