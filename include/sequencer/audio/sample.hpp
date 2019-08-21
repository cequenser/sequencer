#pragma once

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <vector>

namespace sequencer::audio
{
    namespace underlying_t
    {
        using mode_t = std::uint8_t;
    }

    enum class mode_t : underlying_t::mode_t
    {
        mono = 1,
        stereo = 2
    };

    class sample_t
    {
    public:
        sample_t( std::size_t max_frame_index, mode_t mode = mode_t::stereo )
            : max_frame_index_{max_frame_index}, mode_{mode}
        {
        }

        constexpr std::size_t frames_left() const noexcept
        {
            return max_frame_index_ - frame_index_;
        }

        constexpr void reset_frame_index() noexcept
        {
            frame_index_ = 0;
        }

        void read( const float* data, std::size_t frames_per_buffer )
        {
            const auto size = frames_to_copy( frames_per_buffer );
            if ( data != nullptr )
            {
                std::memcpy( current_frame(), data, size * frame_size_in_bytes() );
            }
            increase_frame_index( size );
        }

        void write( float* data, std::size_t frames_per_buffer )
        {
            const auto size = frames_to_copy( frames_per_buffer );
            std::memcpy( data, current_frame(), size * frame_size_in_bytes() );
            increase_frame_index( size );
        }

    private:
        constexpr std::size_t frames_to_copy( std::size_t frames_per_buffer ) const noexcept
        {
            return std::min( frames_left(), frames_per_buffer );
        }

        constexpr void increase_frame_index( unsigned long increment ) noexcept
        {
            frame_index_ += increment;
        }

        float* current_frame()
        {
            return &recorded_samples_[ frame_index_ * channels() ];
        }

        constexpr std::size_t frame_size_in_bytes() const noexcept
        {
            return sizeof( float ) * channels();
        }

        constexpr underlying_t::mode_t channels() const noexcept
        {
            return static_cast< underlying_t::mode_t >( mode_ );
        }

        std::size_t frame_index_{0};
        std::size_t max_frame_index_;
        mode_t mode_;
        std::vector< float > recorded_samples_ =
            std::vector< float >( max_frame_index_ * channels(), 0 );
    };
} // namespace sequencer::audio
