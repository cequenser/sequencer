#pragma once

#include <array>

namespace sequencer::midi
{
    template < unsigned number_of_steps >
    class track
    {
        using container = std::array< int, number_of_steps >;

    public:
        using size_type = typename container::size_type;
        static constexpr int no_note = -1;

        track() noexcept
        {
            clear();
        }

        constexpr unsigned steps() const noexcept
        {
            return number_of_steps;
        }

        int& operator[]( size_type i ) noexcept
        {
            return steps_[ i ];
        }

        int operator[]( size_type i ) const noexcept
        {
            return steps_[ i ];
        }

        void clear() noexcept
        {
            steps_.fill( no_note );
        }

    private:
        container steps_{};
    };
} // namespace sequencer::midi
