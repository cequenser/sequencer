#pragma once

#include <sequencer/assert.hpp>
#include <sequencer/beat_duration.hpp>
#include <sequencer/midi/constants.hpp>
#include <sequencer/midi/message/channel_voice.hpp>
#include <sequencer/midi/message/message_type.hpp>
#include <sequencer/midi/message/realtime.hpp>
#include <sequencer/midi/step.hpp>
#include <sequencer/vector.hpp>

#include <algorithm>
#include <atomic>
#include <functional>
#include <vector>

namespace sequencer::midi
{
    using track_base_t = vector< step_t >;

    inline void copy_track( const track_base_t& from, track_base_t& to ) noexcept
    {
        const auto size = std::min( from.size(), to.size() );
        for ( auto step = std::size_t{0}; step < size; ++step )
        {
            to[ step ] = from[ step ];
        }
    }

    class clock_to_step_t
    {
    public:
        static constexpr int do_not_send = -1;

        clock_to_step_t() = default;

        explicit clock_to_step_t( std::size_t steps ) noexcept : steps_( steps )
        {
            assert( steps_ > 0 );
        }

        constexpr void set_steps( std::size_t new_steps ) noexcept
        {
            assert( new_steps > 0 );
            steps_ = new_steps;
            midi_beat_counter_ %= steps_ * pulses_per_step();
        }

        constexpr void set_steps_per_beat( std::size_t steps ) noexcept
        {
            steps_per_beat_ = steps;
        }

        constexpr std::size_t steps_per_beat() noexcept
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

    template < class Parameter >
    class track_t
    {
    public:
        using value_type = track_base_t::value_type;
        using size_type = track_base_t::size_type;

        explicit track_t( size_type size = 64, size_type initial_size = 16 )
            : track_{size, std::min( size, initial_size )}, clock_to_step_{size}
        {
            clear();
        }

        // NOLINTNEXTLINE(bugprone-copy-constructor-init)
        track_t( const track_t& other )
            : track_{other.track_.size()}, clock_to_step_{other.clock_to_step_}
        {
            copy( other, *this );
        }

        track_t& operator=( const track_t& other )
        {
            if ( this != &other )
            {
                track_ = track_base_t{other.track_.size()};
                clock_to_step_ = other.clock_to_step_;
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
            clock_to_step_.set_steps( new_steps );

            auto new_track = track_base_t( new_steps );
            copy_track( track_, new_track );

            if ( new_steps > steps() )
            {
                for ( size_type new_step = track_.size(); new_step < new_steps; ++new_step )
                {
                    new_track[ new_step ] = ( copy_offset > 0 && new_step >= copy_offset )
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

            const auto& step = track_[ idx ];
            if ( step.is_active() )
            {
                const auto note = get_note( step );
                sender( lfo_( idx, channel::voice::note_on( channel(), to_uint8_t( note ),
                                                            get_velocity( step ) ) ) );
                current_notes_.emplace_back( note, get_length() );
            }
        }

        template < class Sender >
        void send_messages( message_t< 1 > message, const Sender& sender ) const
        {
            const auto step = clock_to_step_.process_message( message );
            if ( process_control_message( message, sender ) || !clock_to_step_.started() )
            {
                return;
            }

            for ( auto& entry : current_notes_ )
            {
                if ( --entry.second == 0 )
                {
                    sender( lfo_( step, channel::voice::note_off(
                                            channel(), to_uint8_t( entry.first ), 0 ) ) );
                }
            }
            if ( step != clock_to_step_t::do_not_send )
            {
                send_messages( step, sender );
            }
        }

        template < class Sender >
        void send_all_notes_off_message( const Sender& sender ) const
        {
            sender( channel::voice::all_notes_off( channel() ) );
            current_notes_.clear();
        }

        void mute( bool do_mute = true ) noexcept
        {
            is_muted_ = do_mute;
        }

        bool is_muted() const noexcept
        {
            return is_muted_;
        }

        constexpr note_t base_note() const noexcept
        {
            return base_note_;
        }

        template < class F >
        void set_lfo( F f )
        {
            lfo_ = f;
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

        Parameter& parameter() noexcept
        {
            return parameter_;
        }

        const Parameter& parameter() const noexcept
        {
            return parameter_;
        }

    private:
        note_t get_note( const step_t& step ) const noexcept
        {
            return step.note() ? step.note()->load() : base_note() + parameter().note_offset();
        }

        std::uint8_t get_velocity( const step_t& step ) const noexcept
        {
            return step.velocity() ? step.velocity()->load() : parameter().velocity();
        }

        std::size_t get_length() const noexcept
        {
            return std::size_t(
                ( parameter().note_length().to_double() / clock_to_step_.steps_per_beat() ) *
                    clock_to_step_.pulses_per_quarter_note() +
                1e-6 );
        }

        void copy( const track_t& from, track_t& to )
        {
            to.channel_ = from.channel();
            to.base_note_ = from.base_note();
            to.is_muted_ = from.is_muted();
            copy_track( from.track_, to.track_ );
        }

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
                send_all_notes_off_message( sender );
                return true;
            }

            return false;
        }

        track_base_t track_;
        Parameter parameter_{};
        mutable clock_to_step_t clock_to_step_;
        std::function< message_t< 3 >( std::size_t, message_t< 3 > ) > lfo_ =
            []( std::size_t, message_t< 3 > msg ) { return msg; };
        std::uint8_t channel_{0};
        note_t base_note_{36};
        std::atomic_bool is_muted_{false};
        mutable std::vector< std::pair< note_t, std::size_t /*remaining_pulses*/ > > current_notes_;
    };

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
