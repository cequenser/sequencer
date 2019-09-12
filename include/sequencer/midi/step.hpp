#pragma once

#include <atomic>
#include <cstdint>
#include <limits>
#include <optional>
#include <ostream>

namespace sequencer::midi
{
    enum class note_t : std::uint8_t
    {
        no_note = std::numeric_limits< std::uint8_t >::max()
    };

    constexpr note_t no_note() noexcept
    {
        return note_t::no_note;
    }

    constexpr std::uint8_t to_uint8_t( note_t note ) noexcept
    {
        return static_cast< std::uint8_t >( note );
    }

    class step_t
    {
    public:
        constexpr explicit step_t( bool active = false ) noexcept : is_active_{active}
        {
        }

        constexpr step_t( note_t note, std::uint8_t velocity ) noexcept
            : is_active_{true}, note_{note}, velocity_{velocity}
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

        constexpr bool is_active() const noexcept
        {
            return is_active_;
        }

        constexpr void set_note( note_t note ) noexcept
        {
            note_ = note;
        }

        constexpr const std::optional< std::atomic< note_t > >& note() const noexcept
        {
            return note_;
        }

        constexpr void set_velocity( std::uint8_t velocity ) noexcept
        {
            velocity_ = velocity;
        }

        constexpr const std::optional< std::atomic< std::uint8_t > >& velocity() const noexcept
        {
            return velocity_;
        }

    private:
        void copy_from( const step_t& other )
        {
            is_active_ = other.is_active();
            if ( other.note_ )
            {
                note_ = other.note_->load();
            }
            else
            {
                note_.reset();
            }
            if ( other.velocity_ )
            {
                velocity_ = other.velocity_->load();
            }
            else
            {
                velocity_.reset();
            }
        }

        std::atomic_bool is_active_{false};
        std::optional< std::atomic< note_t > > note_{};
        std::optional< std::atomic< std::uint8_t > > velocity_{};
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

    constexpr bool operator==( const step_t& lhs, const step_t& rhs ) noexcept
    {
        return lhs.is_active() == rhs.is_active() && lhs.note() == rhs.note() &&
               lhs.velocity() == rhs.velocity();
    }

    constexpr bool operator!=( const step_t& lhs, const step_t& rhs ) noexcept
    {
        return !( lhs == rhs );
    }
} // namespace sequencer::midi
