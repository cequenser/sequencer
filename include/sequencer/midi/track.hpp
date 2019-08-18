#pragma once

#include <sequencer/midi/message/channel_voice.hpp>
#include <sequencer/midi/message/message_type.hpp>
#include <sequencer/midi/message/realtime.hpp>
#include <sequencer/midi/note.hpp>

#include <algorithm>
#include <atomic>
#include <cassert>
#include <vector>

namespace sequencer::midi
{
    using track_base_t = std::vector< std::atomic< note_t > >;

    inline void copy_track( const track_base_t& from, track_base_t& to ) noexcept
    {
        const auto size = std::min( from.size(), to.size() );
        for ( auto step = std::size_t{0}; step < size; ++step )
        {
            to[ step ] = from[ step ].load();
        }
    }

    class track_t
    {
    public:
        using size_type = typename track_base_t::size_type;

        constexpr track_t() noexcept = default;

        explicit track_t( size_type size ) : track_( size )
        {
            clear();
        }

        track_t( const track_t& other )
            : track_{other.track_.size()}, channel_( other.channel_ ), velocity_( other.velocity_ )
        {
            copy_track( other.track_, track_ );
        }

        track_t& operator=( const track_t& other )
        {
            if ( this != &other )
            {
                track_ = track_base_t{other.track_.size()};
                copy_track( other.track_, track_ );
                channel_ = other.channel_;
                velocity_ = other.velocity_;
            }
            return *this;
        }

        std::size_t steps() const noexcept
        {
            return track_.size();
        }

        void set_steps( std::size_t new_steps )
        {
            auto new_track = track_base_t( new_steps );
            copy_track( track_, new_track );

            if ( new_steps > steps() )
            {
                for ( size_type new_step = track_.size(); new_step < new_steps; ++new_step )
                {
                    new_track[ new_step ] = no_note();
                }
            }

            track_ = std::move( new_track );
        }

        std::atomic< note_t >& operator[]( size_type i ) noexcept
        {
            return track_[ i ];
        }

        const std::atomic< note_t >& operator[]( size_type i ) const noexcept
        {
            return track_[ i ];
        }

        void clear() noexcept
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
        track_base_t track_{};
        mutable note_t last_note_{no_note()};
        std::uint8_t channel_{0};
        std::uint8_t velocity_{32};
    };

    template < class Track >
    class real_time_to_step_t : public Track
    {
    public:
        using Track::Track;

        template < class Sender >
        void send_messages( message_t< 1 > message, const Sender& sender ) const
        {
            if ( process_control_message( message, sender ) || !started_ )
            {
                return;
            }

            const auto pulses_per_step = pulses_per_quarter_note_ / steps_per_beat_;
            if ( midi_beat_counter_ % pulses_per_step == 0 )
            {
                const auto step = midi_beat_counter_ / pulses_per_step;
                Track::send_messages( step, sender );
            }
            if ( ++midi_beat_counter_ == Track::steps() * pulses_per_step )
            {
                midi_beat_counter_ = 0;
            }
        }

        constexpr void set_steps_per_beat( std::size_t steps ) noexcept
        {
            steps_per_beat_ = steps;
        }

        constexpr void set_pulses_per_quarter_note( std::size_t pulses_per_quarter_note ) noexcept
        {
            pulses_per_quarter_note_ = pulses_per_quarter_note;
        }

    private:
        template < class Sender >
        bool process_control_message( message_t< 1 > message, const Sender& sender ) const
        {
            if ( message == realtime::realtime_start() )
            {
                started_ = true;
                midi_beat_counter_ = 0;
                return true;
            }
            if ( message == realtime::realtime_continue() )
            {
                started_ = true;
                return true;
            }
            if ( message == realtime::realtime_stop() )
            {
                started_ = false;
                Track::send_all_notes_off_message( sender );
                return true;
            }

            return false;
        }

        mutable std::size_t midi_beat_counter_{0};
        std::size_t steps_per_beat_{4};
        std::size_t pulses_per_quarter_note_{24};
        mutable bool started_{false};
    };

    using sequencer_track_t = real_time_to_step_t< track_t >;

    std::vector< sequencer_track_t > inline make_tracks(
        std::size_t number_of_tracks, std::size_t number_of_steps,
        std::size_t pulses_per_quarter_note = std::size_t{24} ) noexcept
    {
        std::vector< sequencer_track_t > tracks( number_of_tracks,
                                                 sequencer_track_t{number_of_steps} );
        std::uint8_t channel = 0;

        for ( auto& track : tracks )
        {
            track.set_channel( channel++ );
            track.set_pulses_per_quarter_note( pulses_per_quarter_note );
        };

        return tracks;
    }

    template < class Tracks, class Sender >
    void send_messages( Tracks& tracks, message_t< 1 > message, const Sender& sender )
    {
        for ( auto& track : tracks )
        {
            track.send_messages( message, sender );
        }
    }

    template < class Tracks, class Sender >
    void send_all_notes_off_message( Tracks& tracks, const Sender& sender )
    {
        for ( auto& track : tracks )
        {
            track.send_all_notes_off_message( sender );
        }
    }
} // namespace sequencer::midi
