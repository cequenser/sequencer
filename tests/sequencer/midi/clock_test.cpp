#include <sequencer/midi/clock.hpp>
#include <sequencer/midi/message_type.hpp>

#include <catch2/catch.hpp>

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <vector>

SCENARIO( "A midi clock running for 1 beat", "[midi_clock]" )
{
    using namespace sequencer;

    GIVEN( "a midi clock that runs for 1 beat and a test sender" )
    {
        std::vector< midi::message_type > received_messages;
        const auto sender = [&received_messages]( midi::message_type message ) {
            received_messages.push_back( message );
        };
        auto clock = midi::clock{sender, 1.0_beats};

        WHEN( "the clock is started" )
        {
            clock.start();
            clock.run();

            THEN( "one start message, 24 clock messages and one stop message are sent" )
            {

                REQUIRE( received_messages.size() >= 2u + 24u );
                CHECK( received_messages.front() == midi::message_type::realtime_start );
                for ( auto i = 1u; i + 1u < received_messages.size(); ++i )
                {
                    CHECK( received_messages[ i ] == midi::message_type::realtime_clock );
                }
                CHECK( received_messages.back() == midi::message_type::realtime_stop );
            }
        }
    }
}

namespace
{
    struct test_sender
    {
        test_sender( std::vector< sequencer::midi::message_type >& received_messages )
            : received_messages( received_messages )
        {
        }

        void operator()( sequencer::midi::message_type message ) const
        {
            received_messages.push_back( message );
            if ( message == sequencer::midi::message_type::realtime_start )
            {
                std::unique_lock lock( shared->message_mutex );
                ++shared->start_message_count;
                lock.unlock();
                shared->message_received.notify_one();
            }
            if ( message == sequencer::midi::message_type::realtime_continue )
            {
                std::unique_lock lock( shared->message_mutex );
                ++shared->continue_message_count;
                lock.unlock();
                shared->message_received.notify_one();
            }
            if ( message == sequencer::midi::message_type::realtime_stop )
            {
                std::unique_lock lock( shared->message_mutex );
                ++shared->stop_message_count;
                lock.unlock();
                shared->message_received.notify_one();
            }
        }

        std::vector< sequencer::midi::message_type >& received_messages;

        struct shared_data
        {
            std::mutex message_mutex;
            int start_message_count{0};
            int continue_message_count{0};
            int stop_message_count{0};
            std::condition_variable message_received;
        };

        std::shared_ptr< shared_data > shared = std::make_shared< shared_data >();
    };
} // namespace

SCENARIO( "Asynchronous control of midi-clock", "[midi_clock]" )
{
    using namespace sequencer;
    using sender_type = std::function< void( midi::message_type ) >;

    GIVEN( "A midi clock running in a separate thread with a clock controller" )
    {
        auto controller_ready_promise = std::make_shared< std::promise< void > >();
        const auto controller_ready = controller_ready_promise->get_future();

        std::vector< midi::message_type > received_messages;
        auto sender = test_sender{received_messages};

        midi::clock_controller< sender_type > clock_controller;
        const auto clock_done = std::async(
            std::launch::async,
            [&clock_controller, sender, controller_ready = std::move( controller_ready_promise )] {
                const auto clock = std::make_shared< midi::clock< sender_type > >( sender );
                clock_controller = midi::get_controller( clock );
                controller_ready->set_value();
                clock->run();
            } );
        controller_ready.wait();

        WHEN( "clock is started" )
        {
            clock_controller.start_clock();
            {
                std::unique_lock lock( sender.shared->message_mutex );
                sender.shared->message_received.wait(
                    lock, [&sender] { return sender.shared->start_message_count == 1; } );
            }

            WHEN( "clock is stopped" )
            {
                clock_controller.stop_clock();
                {
                    std::unique_lock lock( sender.shared->message_mutex );
                    sender.shared->message_received.wait(
                        lock, [&sender] { return sender.shared->stop_message_count == 1; } );
                }

                THEN( "start and stop messages are sent" )
                {
                    CHECK( received_messages.size() >= 2 );
                    CHECK( received_messages.front() == midi::message_type::realtime_start );
                    CHECK( received_messages.back() == midi::message_type::realtime_stop );

                    received_messages.clear();

                    WHEN( "clock is started again" )
                    {
                        clock_controller.start_clock();
                        {
                            std::unique_lock lock( sender.shared->message_mutex );
                            sender.shared->message_received.wait( lock, [&sender] {
                                return sender.shared->continue_message_count == 1;
                            } );
                        }

                        WHEN( "clock is stopped again" )
                        {
                            clock_controller.stop_clock();
                            {
                                std::unique_lock lock( sender.shared->message_mutex );
                                sender.shared->message_received.wait( lock, [&sender] {
                                    return sender.shared->stop_message_count == 2;
                                } );
                            }

                            THEN( "continue and stop messages are sent" )
                            {
                                CHECK( received_messages.size() >= 2 );
                                CHECK( received_messages.front() ==
                                       midi::message_type::realtime_continue );
                                CHECK( received_messages.back() ==
                                       midi::message_type::realtime_stop );
                            }
                        }
                    }

                    WHEN( "clock is reset and then restarted again" )
                    {
                        clock_controller.reset_clock();
                        clock_controller.start_clock();
                        {
                            std::unique_lock lock( sender.shared->message_mutex );
                            sender.shared->message_received.wait( lock, [&sender] {
                                return sender.shared->start_message_count == 2;
                            } );
                        }

                        WHEN( "clock is stopped again" )
                        {
                            clock_controller.stop_clock();
                            {
                                std::unique_lock lock( sender.shared->message_mutex );
                                sender.shared->message_received.wait( lock, [&sender] {
                                    return sender.shared->stop_message_count == 2;
                                } );
                            }

                            THEN( "start and stop messages are sent" )
                            {
                                CHECK( received_messages.size() >= 2 );
                                CHECK( received_messages.front() ==
                                       midi::message_type::realtime_start );
                                CHECK( received_messages.back() ==
                                       midi::message_type::realtime_stop );
                            }
                        }
                    }
                }
            }
        }

        clock_controller.shut_down_clock();
        clock_done.wait();
    }
}
