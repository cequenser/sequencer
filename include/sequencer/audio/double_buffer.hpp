#pragma once

#include <sequencer/audio/sample.hpp>

#include <array>
#include <atomic>

namespace sequencer::audio
{
    class double_buffer
    {
    public:
        sample_t read_buffer() const
        {
            return *read_buffer_;
        }

        sample_t move_from_buffer() const
        {
            return std::move( *read_buffer_ );
        }

        void write_buffer( sample_t&& buffer ) noexcept
        {
            *write_buffer_ = std::move( buffer );
        }

        void write_buffer( const sample_t& buffer )
        {
            *write_buffer_ = buffer;
        }

        constexpr void swap() noexcept
        {
            read_buffer_.exchange( write_buffer_ );
        }

    private:
        sample_t buffer_1_;
        sample_t buffer_2_;
        std::atomic< sample_t* > read_buffer_{&buffer_1_};
        std::atomic< sample_t* > write_buffer_{&buffer_2_};
    };
} // namespace sequencer::audio
