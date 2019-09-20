#pragma once

#include <sequencer/beat_duration.hpp>
#include <sequencer/midi/track_parameter.hpp>

#include <limits>

namespace sequencer::backend
{
    inline std::array< double, 128 > create_digitakt_length_map()
    {
        std::array< double, 128 > length_map;
        using size_type = std::array< double, 128 >::size_type;
        auto value = 0.125;
        size_type start = 0;
        size_type end = 30;
        const auto add_values = [&]( auto delta ) {
            for ( size_type i = start; i < end; ++i )
            {
                length_map[ i ] = value;
                value += delta;
            }
            start = end;
            end = start + 16;
        };

        add_values( 0.0625 );
        add_values( 0.125 );
        add_values( 0.25 );
        add_values( 0.5 );
        add_values( 1.0 );
        add_values( 2.0 );
        add_values( 4.0 );
        length_map[ 127 ] = std::numeric_limits< double >::max();
        return length_map;
    }

    inline const std::array< double, 128 >& digitakt_length_map()
    {
        static const auto length_map = create_digitakt_length_map();
        return length_map;
    }

    struct digitakt_track_parameter_t : midi::track_parameter_t
    {
        static constexpr auto note_offset_idx = 0u;
        static constexpr auto velocity_idx = 1u;
        static constexpr auto length_idx = 2u;

        std::uint16_t note_offset() const noexcept
        {
            return trig_parameter[ note_offset_idx ];
        }

        std::uint16_t velocity() const noexcept
        {
            return trig_parameter[ velocity_idx ];
        }

        beat_duration note_length() const noexcept
        {
            return beat_duration{digitakt_length_map()[ trig_parameter[ length_idx ] ]};
        }
    };
} // namespace sequencer::backend
