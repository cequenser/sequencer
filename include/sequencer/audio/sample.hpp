#pragma once

#include <cstring>
#include <vector>

namespace sequencer::audio
{
    class sample_t
    {
    public:
        sample_t( std::size_t max_frame_index, std::size_t n_channels = 2 )
            : max_frame_index_( max_frame_index ), n_channels_( n_channels )
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
            const auto frames_to_calc = std::min( frames_left(), frames_per_buffer );
            if ( data )
            {
                std::memcpy( current_frame(), data, frames_to_calc * frame_size_in_bytes() );
            }
            increase_frame_index( frames_to_calc );
        }

        void write( float* data, std::size_t frames_per_buffer )
        {
            const auto size = std::min( frames_left(), frames_per_buffer );
            std::memcpy( data, current_frame(), size * frame_size_in_bytes() );
            increase_frame_index( size );
        }

    private:
        constexpr void increase_frame_index( unsigned long increment ) noexcept
        {
            frame_index_ += increment;
        }

        float* current_frame()
        {
            return &recorded_samples_[ frame_index_ * n_channels_ ];
        }

        constexpr std::size_t frame_size_in_bytes() const noexcept
        {
            return sizeof( float ) * n_channels_;
        }

        std::size_t frame_index_{0};
        std::size_t max_frame_index_;
        std::size_t n_channels_;
        std::vector< float > recorded_samples_ =
            std::vector< float >( max_frame_index_ * n_channels_, 0 );
    };
} // namespace sequencer::audio
