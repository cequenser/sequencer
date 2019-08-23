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
        using frame_rep = float;

        sample_t( std::size_t max_frame_index, mode_t mode = mode_t::stereo )
            : max_frame_index_{max_frame_index}, mode_{mode}
        {
        }

        constexpr void trim() noexcept
        {
            max_frame_index_ = frame_index_;
        }

        constexpr void reset_frame_index() noexcept
        {
            frame_index_ = 0;
        }

        void read( const frame_rep* data, std::size_t frames_per_buffer )
        {
            const auto size = frames_to_copy( frames_per_buffer );
            if ( data != nullptr )
            {
                std::memcpy( current_frame(), data, size * frame_size_in_bytes() );
            }
            increase_frame_index( size );
        }

        void write( frame_rep* data, std::size_t frames_per_buffer )
        {
            const auto size = frames_to_copy( frames_per_buffer );
            std::memcpy( data, current_frame(), size * frame_size_in_bytes() );
            increase_frame_index( size );
        }

        constexpr bool has_frames_left() const noexcept
        {
            return frame_index_ < max_frame_index_;
        }

    private:
        constexpr std::size_t frames_to_copy( std::size_t frames_per_buffer ) const noexcept
        {
            const auto frames_left = max_frame_index_ - frame_index_;
            return std::min( frames_left, frames_per_buffer );
        }

        constexpr void increase_frame_index( unsigned long increment ) noexcept
        {
            frame_index_ += increment;
        }

        frame_rep* current_frame()
        {
            return &recorded_samples_[ frame_index_ * channels() ];
        }

        constexpr std::size_t frame_size_in_bytes() const noexcept
        {
            return sizeof( frame_rep ) * channels();
        }

        constexpr underlying_t::mode_t channels() const noexcept
        {
            return static_cast< underlying_t::mode_t >( mode_ );
        }

        std::size_t frame_index_{0};
        std::size_t max_frame_index_;
        mode_t mode_;
        std::vector< frame_rep > recorded_samples_ =
            std::vector< frame_rep >( max_frame_index_ * channels(), 0 );
    };
} // namespace sequencer::audio
