#pragma once

#include <sequencer/copyable_atomic.hpp>

#include <cassert>
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

    constexpr note_t operator+( note_t note, std::int16_t offset )
    {
        assert( to_uint8_t( note ) + offset < 128 );
        return note_t{std::uint8_t( to_uint8_t( note ) + offset )};
    }

    constexpr std::int16_t get_note_distance( note_t lhs, note_t rhs ) noexcept
    {
        return to_uint8_t( rhs ) - to_uint8_t( lhs );
    }

    class step_t
    {
    public:
        constexpr explicit step_t( bool active = false ) noexcept : is_active_{active}
        {
        }

        constexpr step_t( note_t note, std::uint8_t velocity ) noexcept
            : is_active_{note != no_note()}, note_{note}, velocity_{velocity}
        {
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

    private:
        copyable_atomic< bool > is_active_{false};
        std::optional< copyable_atomic< note_t > > note_{};
        std::optional< copyable_atomic< std::uint8_t > > velocity_{};
        //        std::optional< copyable_atomic<double> > length_{};
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
               lhs.velocity() == rhs.velocity();
    }

    inline bool operator!=( const step_t& lhs, const step_t& rhs ) noexcept
    {
        return !( lhs == rhs );
    }
} // namespace sequencer::midi
