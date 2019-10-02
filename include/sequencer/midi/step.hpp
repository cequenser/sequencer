#pragma once

#include <sequencer/beat_duration.hpp>
#include <sequencer/copyable_atomic.hpp>
#include <sequencer/midi/note.hpp>
#include <sequencer/midi/trig_condition.hpp>

#include <cstdint>
#include <mutex>
#include <optional>
#include <ostream>

namespace sequencer::midi
{
    class step_t
    {
    public:
        constexpr explicit step_t( bool active = false ) noexcept : is_active_{active}
        {
        }

        constexpr explicit step_t( note_t note ) noexcept
            : is_active_{note != no_note()}, note_{note}
        {
        }

        constexpr step_t( note_t note, std::uint8_t velocity ) noexcept
            : is_active_{note != no_note()}, note_{note}, velocity_{velocity}
        {
        }

        constexpr step_t( note_t note, std::uint8_t velocity, beat_duration length ) noexcept
            : is_active_{note != no_note()}, note_{note}, velocity_{velocity}, length_{length}
        {
        }

        step_t( const step_t& other ) noexcept
        {
            copy_from( other );
        }

        step_t& operator=( const step_t& other ) noexcept
        {
            copy_from( other );
            return *this;
        }

        void set_active( bool active ) noexcept
        {
            is_active_ = active;
        }

        bool is_active() const noexcept
        {
            return is_active_;
        }

        void set_note( note_t note ) noexcept
        {
            set_active( note != no_note() );
            note_ = note;
        }

        constexpr const std::optional< copyable_atomic< note_t > >& note() const noexcept
        {
            return note_;
        }

        void set_velocity( std::uint8_t velocity ) noexcept
        {
            velocity_ = velocity;
        }

        constexpr const std::optional< copyable_atomic< std::uint8_t > >& velocity() const noexcept
        {
            return velocity_;
        }

        void set_length( beat_duration length ) noexcept
        {
            length_ = length;
        }

        constexpr const std::optional< copyable_atomic< beat_duration > >& length() const noexcept
        {
            return length_;
        }

        template < class F >
        void set_trig_condition( F f ) noexcept
        {
            std::lock_guard lock{trig_condition_mutex_};
            trig_condition_ = f;
        }

        bool evaluate_trig_condition() const noexcept
        {
            std::lock_guard lock{trig_condition_mutex_};
            return !trig_condition_ || trig_condition_();
        }

    private:
        void copy_from( const step_t& other ) noexcept
        {
            is_active_ = other.is_active();
            note_ = other.note();
            velocity_ = other.velocity();
            length_ = other.length();
            trig_condition_ = other.trig_condition_;
        }

        copyable_atomic< bool > is_active_{false};
        std::optional< copyable_atomic< note_t > > note_{};
        std::optional< copyable_atomic< std::uint8_t > > velocity_{};
        std::optional< copyable_atomic< beat_duration > > length_{};
        mutable std::mutex trig_condition_mutex_{};
        trig_condition_t trig_condition_{};
    };

    inline std::ostream& operator<<( std::ostream& os, const step_t& step )
    {
        return os << "(" << step.is_active() << ", "
                  << ( step.note() ? std::to_string( to_uint8_t( step.note()->load() ) )
                                   : std::string( "-" ) )
                  << ","
                  << ( step.velocity() ? std::to_string( step.velocity()->load() )
                                       : std::string( "-" ) )
                  << ")";
    }

    inline bool operator==( const step_t& lhs, const step_t& rhs ) noexcept
    {
        return lhs.is_active() == rhs.is_active() && lhs.note() == rhs.note() &&
               lhs.velocity() == rhs.velocity() && lhs.length() == rhs.length();
    }

    inline bool operator!=( const step_t& lhs, const step_t& rhs ) noexcept
    {
        return !( lhs == rhs );
    }
} // namespace sequencer::midi
