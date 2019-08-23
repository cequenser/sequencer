#pragma once

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <iterator>
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

    constexpr underlying_t::mode_t channels( mode_t mode ) noexcept
    {
        return static_cast< underlying_t::mode_t >( mode );
    }

    struct sample_t
    {
        using frame_rep = float;
        using size_type = std::vector< frame_rep >::size_type;

        sample_t() = default;

        explicit sample_t( size_type number_of_frames, mode_t mode = mode_t::stereo )
            : frames( number_of_frames * channels( mode ), 0 ), mode( mode )
        {
        }

        void trim()
        {
            using std::begin;
            using std::rbegin;
            using std::rend;
            const auto last_non_zero = std::find_if_not(
                rbegin( frames ), rend( frames ), []( frame_rep value ) { return value == 0.0f; } );
            frames.resize( std::distance( last_non_zero, rend( frames ) ) );
        }

        void clear()
        {
            frames = std::vector< frame_rep >( frames.size(), frame_rep{0} );
        }

        size_type number_of_frames() const noexcept
        {
            return frames.size() / channels( mode );
        }

        std::vector< frame_rep > frames{};
        mode_t mode{mode_t::stereo};
    };

    struct read_write_locks_t
    {
        std::atomic_bool block_reading{false};
        std::atomic_bool block_writing{false};
    };

    template < class Type >
    struct read_write_lockable : Type
    {
        using Type::Type;

        constexpr bool block_reading( bool lock ) noexcept
        {
            if ( locks_.block_writing )
            {
                return false;
            }

            locks_.block_reading = lock;
            return true;
        }

        constexpr bool block_writing( bool lock ) noexcept
        {
            if ( locks_.block_reading )
            {
                return false;
            }

            locks_.block_writing = lock;
            return true;
        }

        constexpr bool reading_is_blocked() const noexcept
        {
            return locks_.block_reading;
        }

        constexpr bool writing_is_blocked() const noexcept
        {
            return locks_.block_writing;
        }

    private:
        read_write_locks_t locks_;
    };

    class sample_read_write_base_t
    {
    public:
        using frame_rep = sample_t::frame_rep;
        using size_type = sample_t::size_type;

        explicit sample_read_write_base_t( read_write_lockable< sample_t >& sample ) noexcept
            : sample_{sample}
        {
        }

        constexpr void reset_frame_index() noexcept
        {
            frame_index_ = 0;
        }

        constexpr bool has_frames_left() const noexcept
        {
            return frame_index_ < sample_.number_of_frames();
        }

    protected:
        constexpr size_type frames_to_copy( size_type frames_per_buffer ) const noexcept
        {
            const auto frames_left = sample_.number_of_frames() - frame_index_;
            return std::min( frames_left, frames_per_buffer );
        }

        constexpr void increase_frame_index( unsigned long increment ) noexcept
        {
            frame_index_ += increment;
        }

        frame_rep* current_frame()
        {
            return &sample_.frames[ frame_index_ * channels( sample_.mode ) ];
        }

        constexpr size_type frame_size_in_bytes() const noexcept
        {
            return sizeof( frame_rep ) * channels( sample_.mode );
        }

        read_write_lockable< sample_t >& sample_;

    private:
        size_type frame_index_{0};
    };

    class sample_writer_t : public sample_read_write_base_t
    {
    public:
        using sample_read_write_base_t::sample_read_write_base_t;

        void write( const frame_rep* data, std::size_t frames_per_buffer )
        {
            if ( sample_.writing_is_blocked() )
            {
                return;
            }

            const auto size = frames_to_copy( frames_per_buffer );
            if ( data != nullptr )
            {
                std::memcpy( current_frame(), data, size * frame_size_in_bytes() );
            }
            increase_frame_index( size );
        }
    };

    class sample_reader_t : public sample_read_write_base_t
    {
    public:
        using sample_read_write_base_t::sample_read_write_base_t;

        void read( frame_rep* data, std::size_t frames_per_buffer )
        {
            if ( sample_.reading_is_blocked() )
            {
                return;
            }

            const auto size = frames_to_copy( frames_per_buffer );
            std::memcpy( data, current_frame(), size * frame_size_in_bytes() );
            increase_frame_index( size );
        }
    };
} // namespace sequencer::audio
