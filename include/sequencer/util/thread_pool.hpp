#pragma once

#include <sequencer/util/thread_safe_queue.hpp>

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

namespace sequencer
{
    template < class Task >
    class thread_pool_t
    {
        using Result = decltype( std::declval< Task >() );

        struct thread_with_queue_t
        {
            std::thread thread{};
            thread_safe_queue_t< Task > task_queue{};
            thread_safe_queue_t< Result > result_queue{};
        };

        using size_type = typename std::vector< thread_with_queue_t >::size_type;

    public:
        explicit thread_pool_t( size_type size ) : threads_( size )
        {
            for ( size_type i = 0; i < size; ++i )
            {
                threads_[ i ].thread = std::thread( [this, i] {
                    while ( !stopped_ )
                    {
                        {
                            std::unique_lock lock{sleep_mutex_};
                            sleep_condition_.wait( lock, [this] { return !sleeping_; } );
                        }

                        const auto task = threads_[ i ].task_queue.try_pop();
                        if ( task )
                        {
                            threads_[ i ].result_queue.push( ( *task )() );
                        }
                    }
                } );
            }
        }

        ~thread_pool_t()
        {
            stopped_ = true;
            for ( auto& entry : threads_ )
            {
                entry.thread.join();
            }
        }

        auto& task_queue( size_type idx ) noexcept
        {
            return threads_[ idx ].task_queue;
        }

        auto& result_queue( size_type idx ) noexcept
        {
            return threads_[ idx ].result_queue;
        }

        void stop()
        {
            std::lock_guard lock{sleep_mutex_};
            sleeping_ = true;
            sleep_condition_.notify_all();
        }

        void start()
        {
            std::lock_guard lock{sleep_mutex_};
            sleeping_ = false;
        }

    private:
        std::vector< thread_with_queue_t > threads_;
        std::condition_variable sleep_condition_;
        std::mutex sleep_mutex_;
        bool sleeping_{false};
        std::atomic_bool stopped_{false};
    };
} // namespace sequencer
