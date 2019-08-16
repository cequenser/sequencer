#pragma once

#include <sequencer/midi/message/channel_mode.hpp>
#include <sequencer/midi/message/channel_voice.hpp>
#include <sequencer/midi/message/message_type.hpp>

#include <array>
#include <atomic>
#include <cassert>

namespace sequencer::midi
{
    template < unsigned number_of_steps >
    class track
    {
        using container = std::array< std::atomic_int, number_of_steps >;

    public:
        using size_type = typename container::size_type;
        static constexpr int no_note = -1;

        track() noexcept
        {
            clear();
        }

        track( const track& other ) noexcept
        {
            for ( auto i = size_type{0}; i < steps(); ++i )
            {
                steps_[ i ] = other.steps_[ i ].load();
            }
        }

        track& operator=( const track& other ) noexcept
        {
            if ( this == &other )
            {
                return *this;
            }
            ( *this ) = track{other};
            return *this;
        }

        constexpr unsigned steps() const noexcept
        {
            return number_of_steps;
        }

        std::atomic_int& operator[]( size_type i ) noexcept
        {
            return steps_[ i ];
        }

        const std::atomic_int& operator[]( size_type i ) const noexcept
        {
            return steps_[ i ];
        }

        void clear() noexcept
        {
            for ( auto& step : steps_ )
            {
                step = no_note;
            }
        }

    private:
        container steps_{};
    };

    template < unsigned number_of_steps >
    struct track_for_step_sequencer
    {
    public:
        using size_type = typename track< number_of_steps >::size_type;
        static constexpr auto no_note = track< number_of_steps >::no_note;

        constexpr unsigned steps() const noexcept
        {
            return number_of_steps;
        }

        std::atomic_int& operator[]( size_type i ) noexcept
        {
            return track_[ i ];
        }

        const std::atomic_int& operator[]( size_type i ) const noexcept
        {
            return track_[ i ];
        }

        void clear() noexcept
        {
            track_.clear();
        }

        constexpr void set_channel( std::uint8_t channel ) noexcept
        {
            assert( channel < 16 );
            channel_ = channel;
        }

        constexpr std::uint8_t channel() const noexcept
        {
            return channel_;
        }

        template < class Sender >
        void send_messages( unsigned step, const Sender& sender ) const
        {
            const auto note = track_[ step ].load();
            if ( note != no_note )
            {
                if ( last_note_ != no_note )
                {
                    sender( channel::voice::note_off( channel(), last_note_, velocity_ ) );
                }
                sender( channel::voice::note_on( channel(), note, velocity_ ) );
                last_note_ = note;
            }
        }

        template < class Sender >
        void send_all_notes_off_message( const Sender& sender ) const
        {
            sender( channel::mode::all_notes_off( channel() ) );
            last_note_ = no_note;
        }

    private:
        track< number_of_steps > track_{};
        mutable int last_note_ = no_note;
        std::uint8_t channel_{0};
        std::uint8_t velocity_{32};
    };

    template < unsigned number_of_steps, unsigned number_of_tracks >
    struct tracks_for_step_sequencer
    {
    public:
        using container =
            std::array< track_for_step_sequencer< number_of_steps >, number_of_tracks >;
        using size_type = typename container::size_type;
        static constexpr auto no_note = track_for_step_sequencer< number_of_steps >::no_note;

        tracks_for_step_sequencer() noexcept
        {
            std::uint8_t channel = 0;
            for_each_track( [&channel]( track_for_step_sequencer< number_of_steps >& track ) {
                track.set_channel( channel++ );
            } );
        }

        constexpr unsigned steps() const noexcept
        {
            return number_of_steps;
        }

        track_for_step_sequencer< number_of_steps >& track( size_type i ) noexcept
        {
            return tracks_[ i ];
        }

        const track_for_step_sequencer< number_of_steps >& track( size_type i ) const noexcept
        {
            return tracks_[ i ];
        }

        void clear() noexcept
        {
            for_each_track(
                []( track_for_step_sequencer< number_of_steps >& track ) { track.clear(); } );
        }

        template < class Sender >
        void send_messages( unsigned step, const Sender& sender ) const
        {
            for_each_track(
                [&sender, step]( const track_for_step_sequencer< number_of_steps >& track ) {
                    track.send_messages( step, sender );
                } );
        }

        template < class Sender >
        void send_all_notes_off_message( const Sender& sender ) const
        {
            for_each_track( [&sender]( const track_for_step_sequencer< number_of_steps >& track ) {
                track.send_all_notes_off_message( sender );
            } );
        }

    private:
        template < class Operation >
        void for_each_track( Operation op )
        {
            for ( auto& track : tracks_ )
            {
                op( track );
            }
        }

        template < class Operation >
        void for_each_track( Operation op ) const
        {
            for ( const auto& track : tracks_ )
            {
                op( track );
            }
        }

        container tracks_{};
    };
} // namespace sequencer::midi
