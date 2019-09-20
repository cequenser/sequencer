#pragma once

#include <array>
#include <atomic>
#include <cstdint>

namespace sequencer::midi
{

    struct track_parameter_t
    {
        track_parameter_t() = default;

        track_parameter_t( const track_parameter_t& other ) noexcept
            : trig_parameter{}, source_parameter{other.source_parameter},
              filter_parameter{other.filter_parameter}, amp_parameter{other.amp_parameter},
              lfo_parameter{other.lfo_parameter}, delay_parameter{other.delay_parameter},
              reverb_parameter{other.reverb_parameter}
        {
            using size_type = std::array< std::atomic< std::uint8_t >, 8 >::size_type;
            for ( size_type i = 0; i < trig_parameter.size(); ++i )
            {
                trig_parameter[ i ] = other.trig_parameter[ i ].load();
            }
        }

        track_parameter_t& operator=( const track_parameter_t& other ) noexcept
        {
            using size_type = std::array< std::atomic< std::uint8_t >, 8 >::size_type;
            for ( size_type i = 0; i < trig_parameter.size(); ++i )
            {
                trig_parameter[ i ] = other.trig_parameter[ i ].load();
            }

            source_parameter = other.source_parameter;
            filter_parameter = other.filter_parameter;
            amp_parameter = other.amp_parameter;
            lfo_parameter = other.lfo_parameter;
            delay_parameter = other.delay_parameter;
            reverb_parameter = other.reverb_parameter;

            return *this;
        }

        track_parameter_t( track_parameter_t&& ) = default;
        track_parameter_t& operator=( track_parameter_t&& ) = default;

        std::array< std::atomic< std::int16_t >, 8 > trig_parameter{0, 100, 14, 0, 1, 1, 1, 0};
        std::array< std::int16_t, 8 > source_parameter{0, 0, 0, 0, 0, 120, 0, 100};
        std::array< std::int16_t, 8 > filter_parameter{0, 0, 0, 0, 127, 0, 0, 0};
        std::array< std::int16_t, 8 > amp_parameter{0, 0, 0, 0, 0, 0, 0, 100};
        std::array< std::int16_t, 8 > lfo_parameter{0, 0, 0, 0, 0, 0, 0, 0};
        std::array< std::int16_t, 8 > delay_parameter{0, 0, 0, 0, 0, 0, 0, 100};
        std::array< std::int16_t, 8 > reverb_parameter{0, 0, 0, 0, 0, 0, 0, 100};
    };
} // namespace sequencer::midi
