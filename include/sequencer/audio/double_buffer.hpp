#pragma once

#include <sequencer/audio/sample.hpp>

#include <algorithm>
#include <atomic>
#include <cstring>

namespace sequencer::audio
{
    class double_buffer_t
    {
    public:
        double_buffer_t() = default;
        double_buffer_t( const double_buffer_t& ) = delete;
        double_buffer_t& operator=( const double_buffer_t& ) = delete;

        void swap_read_buffer( sample_t& buffer ) noexcept
        {
            audio::swap( buffer, *read_buffer_ );
            has_new_data_ = false;
        }

        void swap_write_buffer( sample_t& buffer )
        {
            audio::swap( buffer, *write_buffer_ );
        }

        void read_data() noexcept
        {
            has_new_data_ = false;
        }

        bool has_new_data() const noexcept
        {
            return has_new_data_;
        }

        void swap_buffers() noexcept
        {
            std::swap( read_buffer_, write_buffer_ );
            has_new_data_ = true;
        }

    private:
        sample_t buffer_1_;
        sample_t buffer_2_;
        sample_t* read_buffer_{&buffer_1_};
        sample_t* write_buffer_{&buffer_2_};
        std::atomic_bool has_new_data_{false};
    };

    struct current_buffer_t
    {
    protected:
        read_write_lockable< sample_t > current_buffer_;
    };

    class double_buffer_reader_t : public current_buffer_t, public sample_read_write_base_t
    {
    public:
        using frame_rep = sample_t::frame_rep;
        using size_type = sample_t::size_type;

        explicit double_buffer_reader_t( double_buffer_t& buffer ) noexcept
            : current_buffer_t{}, sample_read_write_base_t{current_buffer_}, buffer_{buffer}
        {
        }

        void read( frame_rep* data, std::size_t frames_per_buffer )
        {
            if ( buffer_.has_new_data() && !has_frames_left() )
            {
                reset_frame_index();
                buffer_.swap_read_buffer( current_buffer_ );
            }

            const auto size = frames_to_copy( frames_per_buffer );
            std::memcpy( data, current_frame(), size * frame_size_in_bytes() );
            increase_frame_index( size );
        }

        constexpr bool continue_reading() const noexcept
        {
            return continue_reading_ || has_frames_left();
        }

        constexpr void set_continue_reading( bool continue_reading ) noexcept
        {
            continue_reading_ = continue_reading;
        }

    private:
        double_buffer_t& buffer_;
        bool continue_reading_ = true;
    };
} // namespace sequencer::audio
