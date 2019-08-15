#pragma once

#include <array>
#include <atomic>

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
} // namespace sequencer::midi
