#pragma once

#include <sequencer/audio/dry_wet.hpp>
#include <sequencer/chrono/units.hpp>

#include <chrono>
#include <mutex>
#include <numeric>
#include <thread>
#include <vector>

namespace sequencer::audio
{
    class delay_t
    {
    public:
        using size_type = std::vector< float >::size_type;

        explicit delay_t( size_type frame_count = 0, float gain = 1.0f )
            : buffer_( frame_count, 0.0f ), gain_{gain}
        {
        }

        void set_frame_count( size_type frame_count )
        {
            std::lock_guard lock{mutex_};
            buffer_.resize( frame_count, 0.0f );
            if ( frame_count > 0 )
            {
                current_ = current_ % buffer_.size();
                reset_unlocked();
            }
        }

        size_type frame_count() const noexcept
        {
            std::lock_guard lock{mutex_};
            return buffer_.size();
        }

        float operator()( float x ) noexcept
        {
            std::lock_guard lock{mutex_};
            if ( buffer_.empty() )
            {
                return 0;
            }

            const auto y = buffer_[ current_ ];
            buffer_[ current_ ] = x;
            current_ = ( ++current_ ) % buffer_.size();
            return gain_ * y;
        }

        void reset() noexcept
        {
            std::lock_guard lock{mutex_};
            reset_unlocked();
        }

        void set_gain( float gain ) noexcept
        {
            gain_ = gain;
        }

        float gain() const noexcept
        {
            return gain_;
        }

        delay_t( const delay_t& other )
        {
            std::lock_guard lock{other.mutex_};
            buffer_ = other.buffer_;
            current_ = other.current_;
            gain_ = other.gain_;
        }

        delay_t& operator=( const delay_t& other )
        {
            if ( this == &other )
            {
                return *this;
            }

            std::lock( mutex_, other.mutex_ );
            std::lock_guard lock{mutex_, std::adopt_lock};
            std::lock_guard lock_other{other.mutex_, std::adopt_lock};

            buffer_ = other.buffer_;
            current_ = other.current_;
            gain_ = other.gain_;
            return *this;
        }

    private:
        void reset_unlocked() noexcept
        {
            for ( auto& entry : buffer_ )
            {
                entry = 0;
            }
        }

        mutable std::mutex mutex_;
        std::vector< float > buffer_{};
        size_type current_{0};
        copyable_atomic< float > gain_{1};
    };

    template < class Delay >
    class repeated_delay_t
    {
    public:
        using size_type = typename Delay::size_type;

        explicit repeated_delay_t( size_type delay_count = 0, size_type frame_count = 0,
                                   float gain = 1.0f )
            : delays_( delay_count, Delay{frame_count, gain} )
        {
        }

        float operator()( float x ) noexcept
        {
            std::lock_guard lock{mutex_};
            return std::accumulate( begin( delays_ ), end( delays_ ), float( 0 ),
                                    [&x]( float z, auto& delay ) {
                                        x = delay( x );
                                        return z += x;
                                    } );
        }

        void set_frame_count( std::vector< float >::size_type frame_count ) noexcept
        {
            std::lock_guard lock{mutex_};
            for ( auto& delay : delays_ )
            {
                delay.set_frame_count( frame_count );
            }
        }

        void reset() noexcept
        {
            std::lock_guard lock{mutex_};
            for ( auto& delay : delays_ )
            {
                delay.reset();
            }
        }

        void set_gain( float gain ) noexcept
        {
            std::lock_guard lock{mutex_};
            for ( auto& delay : delays_ )
            {
                delay.set_gain( gain );
            }
        }

        void set_delay_count( std::vector< delay_t >::size_type count )
        {
            std::lock_guard lock{mutex_};
            const auto non_empty = !delays_.empty();

            delays_.resize( count, delay_t{non_empty ? delays_.front().frame_count() : 0,
                                           non_empty ? delays_.front().gain() : 1.0f} );
        }

    private:
        std::mutex mutex_;
        std::vector< Delay > delays_{};
    };

    template < class Delay >
    class stereo_delay_t : public Delay
    {
    public:
        using Delay::Delay;

        std::pair< float, float > operator()( float x ) noexcept
        {
            const auto y = Delay::operator()( x );
            return {( 1.0f - ratio_ ) * y, ratio_ * y};
        }

        void set_stereo_ratio( float ratio ) noexcept
        {
            ratio_ = ratio;
        }

    private:
        float ratio_{0.5};
    };

    using stereo_repeated_delay_t = stereo_delay_t< repeated_delay_t< delay_t > >;
} // namespace sequencer::audio
