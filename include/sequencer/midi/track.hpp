#pragma once

#include <sequencer/assert.hpp>
#include <sequencer/midi/constants.hpp>
#include <sequencer/midi/message/channel_voice.hpp>
#include <sequencer/midi/message/message_type.hpp>
#include <sequencer/midi/message/realtime.hpp>
#include <sequencer/midi/step.hpp>

#include <algorithm>
#include <atomic>
#include <functional>
#include <mutex>
#include <vector>

namespace sequencer::midi
{
    using track_base_t = std::vector< step_t >;

    inline void copy_track( const track_base_t& from, track_base_t& to ) noexcept
    {
        const auto size = std::min( from.size(), to.size() );
        for ( auto step = std::size_t{0}; step < size; ++step )
        {
            to[ step ] = from[ step ];
        }
    }

    struct track_parameter_t
    {
        std::array< std::uint8_t, 8 > trig_parameter{0, 100, 0, 0, 1, 1, 1, 0};
        std::array< std::uint8_t, 8 > source_parameter{0, 0, 0, 0, 0, 120, 0, 100};
        std::array< std::uint8_t, 8 > filter_parameter{0, 0, 0, 0, 127, 0, 0, 0};
        std::array< std::uint8_t, 8 > amp_parameter{0, 0, 0, 0, 0, 0, 0, 100};
        std::array< std::uint8_t, 8 > lfo_parameter{0, 0, 0, 0, 0, 0, 0, 0};
        std::array< std::uint8_t, 8 > delay_parameter{0, 0, 0, 0, 0, 0, 0, 100};
        std::array< std::uint8_t, 8 > reverb_parameter{0, 0, 0, 0, 0, 0, 0, 100};
    };

    class track_t : public track_parameter_t
    {
    public:
        using value_type = track_base_t::value_type;
        using size_type = track_base_t::size_type;

        track_t() = default;

        explicit track_t( size_type size ) : track_( size )
        {
            clear();
        }

        track_t( const track_t& other ) : track_{other.track_.size()}
        {
            std::lock_guard lock( other.mutex_ );
            copy( other, *this );
        }

        track_t& operator=( const track_t& other )
        {
            if ( this != &other )
            {
                std::lock( mutex_, other.mutex_ );
                std::lock_guard lock{mutex_, std::adopt_lock};
                std::lock_guard lock_other{other.mutex_, std::adopt_lock};

                track_ = track_base_t{other.track_.size()};
                copy( other, *this );
            }
            return *this;
        }

        std::size_t steps() const noexcept
        {
            return track_.size();
        }

        void set_steps( std::size_t new_steps, std::size_t copy_offset = 0 )
        {
            std::lock_guard lock( mutex_ );
            auto new_track = track_base_t( new_steps );
            copy_track( track_, new_track );

            if ( new_steps > steps() )
            {
                for ( size_type new_step = track_.size(); new_step < new_steps; ++new_step )
                {
                    new_track[ new_step ] = ( copy_offset && new_step >= copy_offset )
                                                ? new_track[ new_step - copy_offset ]
                                                : step_t{};
                }
            }

            track_ = std::move( new_track );
        }

        value_type& operator[]( size_type i ) noexcept
        {
            return track_[ i ];
        }

        const value_type& operator[]( size_type i ) const noexcept
        {
            return track_[ i ];
        }

        void clear() noexcept
        {
            std::lock_guard lock( mutex_ );
            for ( auto& step : track_ )
            {
                step = step_t{};
            }
        }

        constexpr void set_channel( std::uint8_t channel ) noexcept
        {
            SEQUENCER_ASSERT( channel < max_number_of_midi_channels );
            channel_ = channel;
        }

        constexpr std::uint8_t channel() const noexcept
        {
            return channel_;
        }

        template < class Sender >
        void send_messages( std::size_t idx, const Sender& sender ) const
        {
            if ( is_muted() )
            {
                return;
            }

            std::lock_guard lock( mutex_ );
            const auto& step = track_[ idx ];
            if ( step.is_active() )
            {
                if ( last_step_.is_active() )
                {
                    sender( lfo_( idx, channel::voice::note_off( channel(), get_note( last_step_ ),
                                                                 get_velocity( last_step_ ) ) ) );
                }
                sender( lfo_( idx, channel::voice::note_on( channel(), get_note( step ),
                                                            get_velocity( step ) ) ) );
                last_step_ = step;
            }
        }

        template < class Sender >
        void send_all_notes_off_message( const Sender& sender ) const
        {
            sender( channel::voice::all_notes_off( channel() ) );
            last_step_ = step_t{};
        }

        void mute( bool do_mute = true ) noexcept
        {
            is_muted_ = do_mute;
        }

        bool is_muted() const noexcept
        {
            return is_muted_;
        }

        void set_note_offset( std::uint8_t offset ) noexcept
        {
            note_offset_ = offset;
        }

        std::uint8_t note_offset() const noexcept
        {
            return note_offset_;
        }

        void set_velocity( std::uint8_t velocity ) noexcept
        {
            velocity_ = velocity;
        }

        constexpr note_t base_note() const noexcept
        {
            return base_note_;
        }

        note_t note() const noexcept
        {
            return base_note() + note_offset();
        }

        std::uint8_t velocity() const noexcept
        {
            return velocity_;
        }

        template < class F >
        void set_lfo( F f )
        {
            lfo_ = f;
        }

    private:
        std::uint8_t get_note( const step_t& step ) const noexcept
        {
            return step.note() ? to_uint8_t( step.note()->load() )
                               : ( to_uint8_t( base_note_ ) + note_offset_ );
        }

        std::uint8_t get_velocity( const step_t& step ) const noexcept
        {
            return step.velocity() ? step.velocity()->load() : velocity();
        }

        void copy( const track_t& from, track_t& to )
        {
            to.channel_ = from.channel();
            to.base_note_ = from.base_note();
            to.note_offset_ = from.note_offset();
            to.velocity_ = from.velocity();
            to.is_muted_ = from.is_muted();
            copy_track( from.track_, to.track_ );
        }

        track_base_t track_{};
        mutable step_t last_step_{};
        mutable std::mutex mutex_;
        std::function< message_t< 3 >( std::size_t, message_t< 3 > ) > lfo_ =
            []( std::size_t, message_t< 3 > msg ) { return msg; };
        std::uint8_t channel_{0};
        note_t base_note_{64};
        std::atomic< std::uint8_t > note_offset_{0};
        std::atomic< std::uint8_t > velocity_{100};
        std::atomic_bool is_muted_{false};
    };

    class clock_to_step_impl
    {
    public:
        static constexpr int do_not_send = -1;

        constexpr void set_steps( std::size_t new_steps ) noexcept
        {
            steps_ = new_steps;
            midi_beat_counter_ %= steps_ * pulses_per_step();
        }

        constexpr void set_steps_per_beat( std::size_t steps ) noexcept
        {
            steps_per_beat_ = steps;
        }

        constexpr std::size_t set_steps_per_beat() noexcept
        {
            return steps_per_beat_;
        }

        constexpr void set_pulses_per_quarter_note( std::size_t pulses_per_quarter_note ) noexcept
        {
            pulses_per_quarter_note_ = pulses_per_quarter_note;
            midi_beat_counter_ %= steps_ * pulses_per_step();
        }

        void reset_beat_counter() noexcept
        {
            midi_beat_counter_ = 0;
        }

        void set_midi_beat_counter( std::size_t counter ) noexcept
        {
            midi_beat_counter_ = counter;
        }

        constexpr std::size_t midi_beat_counter() const noexcept
        {
            return midi_beat_counter_;
        }

        constexpr std::size_t pulses_per_quarter_note() const noexcept
        {
            return pulses_per_quarter_note_;
        }

        constexpr std::size_t pulses_per_step() const noexcept
        {
            return pulses_per_quarter_note() / steps_per_beat_;
        }

        int process_message( message_t< 1 > message ) const
        {
            if ( process_control_message( message ) || !started_ )
            {
                return do_not_send;
            }

            auto result = do_not_send;
            if ( midi_beat_counter_ % pulses_per_step() == 0 )
            {
                result = int( midi_beat_counter_ / pulses_per_step() );
            }
            if ( ++midi_beat_counter_ == steps_ * pulses_per_step() )
            {
                midi_beat_counter_ = 0;
            }

            return result;
        }

        constexpr bool started() const noexcept
        {
            return started_;
        }

    private:
        bool process_control_message( message_t< 1 > message ) const
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
                return true;
            }

            return false;
        }

        mutable std::size_t midi_beat_counter_{0};
        std::size_t steps_per_beat_{4};
        std::size_t pulses_per_quarter_note_{default_pulses_per_quarter_note};
        std::size_t steps_{16};
        mutable bool started_{false};
    };

    template < class Track >
    class clock_to_step_t : public Track
    {
    public:
        using size_type = typename Track::size_type;

        clock_to_step_t() = default;

        explicit clock_to_step_t( size_type size ) : Track{size}
        {
            clock_to_step_.set_steps( size );
        }

        template < class Sender >
        void send_messages( message_t< 1 > message, const Sender& sender ) const
        {
            const auto step = clock_to_step_.process_message( message );
            if ( process_control_message( message, sender ) || !clock_to_step_.started() )
            {
                return;
            }

            if ( step != clock_to_step_impl::do_not_send )
            {
                Track::send_messages( step, sender );
            }
        }

        void set_steps( std::size_t new_steps, std::size_t copy_offset = 0 )
        {
            clock_to_step_.set_steps( new_steps );
            Track::set_steps( new_steps, copy_offset );
        }

        constexpr void set_steps_per_beat( std::size_t steps ) noexcept
        {
            clock_to_step_.set_steps_per_beat( steps );
        }

        constexpr void set_pulses_per_quarter_note( std::size_t pulses_per_quarter_note ) noexcept
        {
            clock_to_step_.set_pulses_per_quarter_note( pulses_per_quarter_note );
        }

        constexpr void reset_beat_counter() noexcept
        {
            clock_to_step_.reset_beat_counter();
        }

    private:
        template < class Sender >
        bool process_control_message( message_t< 1 > message, const Sender& sender ) const
        {
            if ( message == realtime::realtime_start() )
            {
                return true;
            }
            if ( message == realtime::realtime_continue() )
            {
                return true;
            }
            if ( message == realtime::realtime_stop() )
            {
                Track::send_all_notes_off_message( sender );
                return true;
            }

            return false;
        }

        mutable clock_to_step_impl clock_to_step_;
    };

    using sequencer_track_t = clock_to_step_t< track_t >;

    std::vector< sequencer_track_t > inline make_tracks(
        std::size_t number_of_tracks, std::size_t number_of_steps,
        std::size_t pulses_per_quarter_note = default_pulses_per_quarter_note ) noexcept
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

    template < class Track, class Sender >
    void send_messages( std::vector< Track >& tracks, message_t< 1 > message, const Sender& sender )
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
