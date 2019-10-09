#pragma once

#include <cassert>
#include <cmath>

namespace sequencer::audio
{
    enum class half_note_t : int
    {
    };

    constexpr int to_int( half_note_t half_note ) noexcept
    {
        return static_cast< int >( half_note );
    }

    inline double increase_by_half_notes( double frequency, half_note_t half_notes ) noexcept
    {
        return frequency * std::pow( 2, to_int( half_notes ) / 12.0 );
    }

    inline double increase_by_half_notes( double frequency, double half_notes ) noexcept
    {
        return frequency * std::pow( 2, half_notes / 12 );
    }

    namespace base_notes
    {
        // clang-format off
        constexpr auto A   = 440.0;
        const auto Ais = increase_by_half_notes(A, half_note_t{1});
        const auto B   = increase_by_half_notes(A, half_note_t{2});
        const auto C   = increase_by_half_notes(A, half_note_t{3});
        const auto Cis = increase_by_half_notes(A, half_note_t{4});
        const auto D   = increase_by_half_notes(A, half_note_t{5});
        const auto Dis = increase_by_half_notes(A, half_note_t{6});
        const auto E   = increase_by_half_notes(A, half_note_t{7});
        const auto F   = increase_by_half_notes(A, half_note_t{8});
        const auto Fis = increase_by_half_notes(A, half_note_t{9});
        const auto G   = increase_by_half_notes(A, half_note_t{10});
        const auto Gis = increase_by_half_notes(A, half_note_t{11});
        // clang-format on
    } // namespace base_notes

    class note_t
    {
    public:
        constexpr note_t() noexcept = default;

        constexpr explicit note_t( double base_note, int octave_offset = 0 ) noexcept
            : base_note_( base_note )
        {
            set_octave_offset( octave_offset );
        }

        constexpr void set_base_note( double base_note ) noexcept
        {
            base_note_ = base_note;
        }

        constexpr void set_octave_offset( int octave_offset ) noexcept
        {
            octave_multiplier_ = std::pow( 2, octave_offset );
        }

        constexpr double frequency() const noexcept
        {
            return base_note_ * octave_multiplier_;
        }

    private:
        double base_note_{base_notes::A};
        double octave_multiplier_{1};
    };

    inline double get_harmonic( double frequency, int n )
    {
        assert( n > 0 );
        return n * frequency;
    }
} // namespace sequencer::audio
