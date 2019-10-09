#pragma once

#include <deque>
#include <mutex>
#include <optional>

namespace sequencer
{
    template < class T, typename std::deque< T >::size_type max_size =
                            std::numeric_limits< typename std::deque< T >::size_type >::max() >
    class thread_safe_queue_t
    {
    public:
        using size_type = typename std::deque< T >::size_type;

        thread_safe_queue_t() = default;

        thread_safe_queue_t( const thread_safe_queue_t& ) = delete;

        thread_safe_queue_t( thread_safe_queue_t&& other ) = delete;

        thread_safe_queue_t& operator=( const thread_safe_queue_t& ) = delete;

        thread_safe_queue_t& operator=( thread_safe_queue_t&& other ) = delete;

        bool push( const T& entry )
        {
            std::lock_guard< std::mutex > lock( m_mutex );

            if ( m_queue.size() == max_size )
                return false;

            m_queue.push_back( entry );
            return true;
        }

        bool push( T&& entry )
        {
            std::lock_guard< std::mutex > lock( m_mutex );

            if ( m_queue.size() == max_size )
                return false;

            m_queue.push_back( std::move( entry ) );
            return true;
        }

        template < class... Args >
        bool emplace( Args&&... args )
        {
            std::lock_guard< std::mutex > lock( m_mutex );

            if ( m_queue.size() == max_size )
                return false;

            m_queue.emplace_back( std::forward< Args >( args )... );
            return true;
        }

        std::optional< T > try_pop()
        {
            std::lock_guard< std::mutex > lock( m_mutex );
            if ( m_queue.empty() )
            {
                return {};
            }

            auto entry = std::move( m_queue.front() );
            m_queue.pop_front();
            return entry;
        }

        void clear()
        {
            std::lock_guard< std::mutex > lock( m_mutex );
            m_queue.clear();
        }

    private:
        std::mutex m_mutex;
        std::deque< T > m_queue;
    };
} // namespace sequencer
