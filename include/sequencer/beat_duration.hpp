#pragma once

#include <sequencer/beat_tempo.hpp>

#include <cassert>
#include <chrono>

namespace sequencer::chrono
{

    class beat_duration
    {
        using minutes = std::chrono::duration< double, std::ratio< 60, 1 > >;

    public:
        using rep = double;
        using seconds = std::chrono::duration< double, std::ratio< 1, 1 > >;

        constexpr explicit beat_duration( rep beats, beat_tempo tempo )
            : beats_( beats ), tempo_( tempo )
        {
        }

        template < typename Duration >
        constexpr explicit beat_duration( Duration duration, beat_tempo tempo )
            : beats_( std::chrono::duration_cast< minutes >( duration ).count() *
                      tempo.beats_per_minute() ),
              tempo_( tempo )
        {
        }

        constexpr rep beats() const noexcept
        {
            return beats_;
        }

        constexpr beat_tempo tempo() const noexcept
        {
            return tempo_;
        }

        constexpr seconds duration() const noexcept
        {
            return minutes{beats_ / tempo_.beats_per_minute()};
        }

    private:
        rep beats_;
        beat_tempo tempo_;
    };

    static_assert( beat_duration( std::chrono::seconds( 30 ), 120.0_bpm ).beats() == 60.0 );
    static_assert(
        beat_duration( std::chrono::seconds( 30 ), 120.0_bpm ).tempo().beats_per_minute() ==
        120.0 );
    static_assert( beat_duration( std::chrono::seconds( 30 ), 120.0_bpm ).duration() ==
                   std::chrono::seconds( 30 ) );

} // namespace sequencer::chrono
