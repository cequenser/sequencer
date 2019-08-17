#pragma once

#include <sequencer/midi/message/channel_voice.hpp>
#include <sequencer/midi/note.hpp>

#include <array>
#include <atomic>
#include <cassert>

namespace sequencer::midi
{
    template < std::size_t number_of_steps >
    using track_base_t = std::array< std::atomic< note_t >, number_of_steps >;

    template < std::size_t number_of_steps >
    void copy_track( const track_base_t< number_of_steps >& from,
                     track_base_t< number_of_steps >& to )
    {
        for ( auto step = std::size_t{0}; step < number_of_steps; ++step )
        {
            to[ step ] = from[ step ].load();
        }
    }

    template < std::size_t number_of_steps >
    struct track_t
    {
    public:
        using size_type = typename track_base_t< number_of_steps >::size_type;

        constexpr track_t() noexcept
        {
            clear();
        };

        track_t( const track_t& other ) noexcept
            : channel_( other.channel_ ), velocity_( other.velocity_ )
        {
            copy_track( other.track_, track_ );
        }

        track_t& operator=( const track_t& other ) noexcept
        {
            if ( this != &other )
            {
                copy_track( other.track_, track_ );
                channel_ = other.channel_;
                velocity_ = other.velocity_;
            }
            return *this;
        }

        constexpr std::size_t steps() const noexcept
        {
            return number_of_steps;
        }

        std::atomic< note_t >& operator[]( size_type i ) noexcept
        {
            return track_[ i ];
        }

        const std::atomic< note_t >& operator[]( size_type i ) const noexcept
        {
            return track_[ i ];
        }

        constexpr void clear() noexcept
        {
            for ( auto& note : track_ )
            {
                note = no_note();
            }
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
        void send_messages( std::size_t step, const Sender& sender ) const
        {
            const auto note = track_[ step ].load();
            if ( note != no_note() )
            {
                if ( last_note_ != no_note() )
                {
                    sender( channel::voice::note_off( channel(), to_uint8_t( last_note_ ),
                                                      velocity_ ) );
                }
                sender( channel::voice::note_on( channel(), to_uint8_t( note ), velocity_ ) );
                last_note_ = note;
            }
        }

        template < class Sender >
        void send_all_notes_off_message( const Sender& sender ) const
        {
            sender( channel::voice::all_notes_off( channel() ) );
            last_note_ = no_note();
        }

    private:
        track_base_t< number_of_steps > track_{};
        mutable note_t last_note_ = no_note();
        std::uint8_t channel_{0};
        std::uint8_t velocity_{32};
    };

    template < std::size_t number_of_steps, std::size_t number_of_tracks >
    struct tracks_t
    {
    public:
        using container = std::array< track_t< number_of_steps >, number_of_tracks >;
        using size_type = typename container::size_type;

        tracks_t() noexcept
        {
            std::uint8_t channel = 0;
            for_each_track( [&channel]( track_t< number_of_steps >& track ) {
                track.set_channel( channel++ );
            } );
        }

        constexpr std::size_t steps() const noexcept
        {
            return number_of_steps;
        }

        track_t< number_of_steps >& operator[]( size_type i ) noexcept
        {
            return tracks_[ i ];
        }

        const track_t< number_of_steps >& operator[]( size_type i ) const noexcept
        {
            return tracks_[ i ];
        }

        void clear() noexcept
        {
            for_each_track( []( track_t< number_of_steps >& track ) { track.clear(); } );
        }

        template < class Sender >
        void send_messages( std::size_t step, const Sender& sender ) const
        {
            for_each_track( [&sender, step]( const track_t< number_of_steps >& track ) {
                track.send_messages( step, sender );
            } );
        }

        template < class Sender >
        void send_all_notes_off_message( const Sender& sender ) const
        {
            for_each_track( [&sender]( const track_t< number_of_steps >& track ) {
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

    template < std::size_t number_of_steps, std::size_t number_of_tracks >
    constexpr tracks_t< number_of_steps, number_of_tracks > make_tracks() noexcept
    {
        return {};
    }
} // namespace sequencer::midi
