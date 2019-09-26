#pragma once

#include <sequencer/copyable_atomic.hpp>

#include <cstdint>
#include <vector>

namespace sequencer::midi
{
    struct track_parameter_t
    {
        using parameters_type = std::vector< copyable_atomic< std::int16_t > >;
        using container_type = std::vector< parameters_type >;
        using size_type = container_type::size_type;

        track_parameter_t() = default;

        explicit track_parameter_t( size_type section_count, size_type parameter_count = 0 )
            : values{section_count, std::vector< copyable_atomic< std::int16_t > >{parameter_count}}
        {
        }

        auto& operator[]( size_type i ) noexcept
        {
            return values[ i ];
        }

        const auto& operator[]( size_type i ) const noexcept
        {
            return values[ i ];
        }

        size_type size() const noexcept
        {
            return values.size();
        }

        void resize( size_type size ) noexcept
        {
            values.resize( size );
        }

        container_type values;
    };
} // namespace sequencer::midi
