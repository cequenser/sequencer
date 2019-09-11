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

        void set_pulses_per_quarter_note( std::size_t pulses_per_quarter )
        {
            for ( auto& track : tracks_ )
            {
                track.set_pulses_per_quarter_note( pulses_per_quarter );
            }
        }

    private:
        std::vector< value_type > tracks_;
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
