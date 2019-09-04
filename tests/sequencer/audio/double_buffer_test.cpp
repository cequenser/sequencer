#include <sequencer/audio/double_buffer.hpp>

#include <catch2/catch.hpp>

#include <future>

SCENARIO( "double_buffer", "[double_buffer]" )
{
    using namespace sequencer::audio;

    GIVEN( "a double buffer" )
    {
        auto double_buffer = double_buffer_t{};

        THEN( "has_new_data() returns false" )
        {
            CHECK_FALSE( double_buffer.has_new_data() );
        }

        WHEN( "swap_read_buffer is called" )
        {
            sample_t buffer;
            double_buffer.swap_read_buffer( buffer );

            THEN( "an empty buffer is written" )
            {
                CHECK( buffer.frames.empty() );
            }

            THEN( "has_new_data() returns false" )
            {
                CHECK_FALSE( double_buffer.has_new_data() );
            }
        }

        WHEN( "swap_write_buffer_is called with a buffer containing 1.f" )
        {
            sample_t buffer;
            buffer.frames = {1.f};
            double_buffer.swap_write_buffer( buffer );

            THEN( "has_new_data() returns false" )
            {
                CHECK_FALSE( double_buffer.has_new_data() );
            }

            AND_WHEN( "swap_buffers is called" )
            {
                double_buffer.swap_buffers();

                THEN( "has_new_data() returns true" )
                {
                    CHECK( double_buffer.has_new_data() );
                }

                AND_WHEN( "swap_read_buffer is called" )
                {
                    sample_t buffer;
                    buffer.frames = {2.f};
                    double_buffer.swap_read_buffer( buffer );

                    THEN( "has_new_data() returns false" )
                    {
                        CHECK_FALSE( double_buffer.has_new_data() );
                    }

                    THEN( "a buffer containing 1.f is written" )
                    {
                        REQUIRE( buffer.frames.size() == 1 );
                        CHECK( buffer.frames.front() == 1.f );
                    }

                    AND_WHEN( "swap_write_buffer is called" )
                    {
                        sample_t buffer;
                        double_buffer.swap_write_buffer( buffer );

                        THEN( "previous write buffer is empty" )
                        {
                            CHECK( buffer.frames.empty() );
                        }
                    }
                }
            }
        }
    }
}

namespace
{
    struct thread_control_t
    {
        std::promise< void > start_thread;
        std::future< void > start_reader_thread = start_thread.get_future();
        std::promise< void > thread_ready;
        std::future< void > thread_is_ready = thread_ready.get_future();
        std::atomic_bool stop_thread{false};
    };
} // namespace

SCENARIO( "asynchronous read-write", "[double_buffer]" )
{
    using namespace sequencer::audio;

    GIVEN( "a double buffer" )
    {
        auto double_buffer = double_buffer_t{};

        WHEN( "buffer is populated in this thread and read in another thread" )
        {
            thread_control_t control;
            std::vector< sample_t > received_samples;
            auto reader_done = std::async(
                std::launch::async, [&control, &received_samples, &double_buffer]() mutable {
                    control.thread_ready.set_value();
                    control.start_reader_thread.wait();

                    while ( !control.stop_thread || double_buffer.has_new_data() )
                    {
                        if ( double_buffer.has_new_data() )
                        {
                            sample_t sample{};
                            double_buffer.swap_read_buffer( sample );
                            received_samples.push_back( sample );
                        }
                    }
                } );

            control.thread_is_ready.wait();
            constexpr auto buffer_size = 1024u;
            constexpr auto buffer_count = 32u;
            read_write_lockable< sample_t > s{buffer_count * buffer_size};
            float value = 1;
            auto counter = 0u;
            std::generate( begin( s.frames ), end( s.frames ), [&counter, &value] {
                if ( ++counter > 2 * buffer_size )
                {
                    counter = 1;
                    ++value;
                }
                return value;
            } );
            auto sample_reader = sample_reader_t{s};
            sample_t buffer{buffer_size};
            sample_reader.read( buffer.frames.data(), buffer.number_of_frames() );
            double_buffer.swap_write_buffer( buffer );
            double_buffer.swap_buffers();
            buffer.frames.resize( 2 * buffer_size );
            buffer.clear();
            sample_reader.read( buffer.frames.data(), buffer.number_of_frames() );
            double_buffer.swap_write_buffer( buffer );

            control.start_thread.set_value();

            while ( true )
            {
                if ( !double_buffer.has_new_data() )
                {
                    double_buffer.swap_buffers();
                    if ( !sample_reader.has_frames_left() )
                    {
                        control.stop_thread = true;
                        break;
                    }

                    buffer.frames.resize( 2 * buffer_size );
                    buffer.clear();
                    sample_reader.read( buffer.frames.data(), buffer.number_of_frames() );
                    double_buffer.swap_write_buffer( buffer );
                }
            }

            reader_done.wait();

            THEN( "32 samples are received" )
            {
                CHECK( received_samples.size() == buffer_count );
            }

            THEN( "all received samples have 1024 frames" )
            {
                CHECK( std::all_of( begin( received_samples ), end( received_samples ),
                                    []( const auto& sample ) {
                                        return sample.number_of_frames() == buffer_size;
                                    } ) );
            }

            THEN( "the received samples are parts of the original signal with same value" )
            {
                for ( auto i = 0u; i < buffer_count; ++i )
                {
                    CHECK( received_samples[ i ].frames.front() == float( i + 1 ) );
                    CHECK( received_samples[ i ].frames.back() == float( i + 1 ) );
                }
            }
        }
    }
}
