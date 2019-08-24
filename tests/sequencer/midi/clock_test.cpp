#include <sequencer/chrono/chrono_adapter.hpp>
#include <sequencer/chrono/sequencer_clock.hpp>
#include <sequencer/chrono/testing_clock.hpp>
#include <sequencer/midi/clock.hpp>
#include <sequencer/midi/message/realtime.hpp>
#include <sequencer/midi/util.hpp>

#include <catch2/catch.hpp>

#include <chrono>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <vector>

using sequencer::midi::make_midi_clock_raii_shutdown;
using sequencer::midi::message_t;
using sequencer::midi::realtime::realtime_clock;
using sequencer::midi::realtime::realtime_continue;
using sequencer::midi::realtime::realtime_start;
using sequencer::midi::realtime::realtime_stop;

namespace
{
    struct message_counting_sender
    {
        message_counting_sender( std::vector< message_t< 1 > >& received_messages )
            : received_messages( received_messages )
        {
        }

        void operator()( message_t< 1 > message ) const
        {
            received_messages.push_back( message );
            if ( message == realtime_start() )
            {
                std::unique_lock lock( shared->message_mutex );
                ++shared->start_message_count;
                lock.unlock();
                shared->message_received.notify_one();
            }
            if ( message == realtime_continue() )
            {
                std::unique_lock lock( shared->message_mutex );
                ++shared->continue_message_count;
                lock.unlock();
                shared->message_received.notify_one();
            }
            if ( message == realtime_stop() )
            {
                std::unique_lock lock( shared->message_mutex );
                ++shared->stop_message_count;
                lock.unlock();
                shared->message_received.notify_one();
            }
            if ( message == realtime_clock() )
            {
                std::unique_lock lock( shared->message_mutex );
                ++shared->clock_message_count;
                lock.unlock();
                shared->message_received.notify_one();
            }
        }

        std::vector< message_t< 1 > >& received_messages;

        struct shared_data
        {
            std::mutex message_mutex;
            std::condition_variable message_received;
            int start_message_count{0};
            int continue_message_count{0};
            int stop_message_count{0};
            int clock_message_count{0};
        };

        std::shared_ptr< shared_data > shared = std::make_shared< shared_data >();
    };
} // namespace

SCENARIO( "A midi clock running for 1 beat", "[midi_clock]" )
{
    using namespace sequencer;
    using namespace std::literals::chrono_literals;

    using underlying_clock_type = chrono::steady_testing_clock<>;
    using sequencer_clock_type = chrono::sequencer_clock< const underlying_clock_type& >;

    GIVEN( "a midi clock that runs for 1 beat and a test sender" )
    {
        auto thread_ready_promise = std::make_shared< std::promise< void > >();
        const auto thread_ready = thread_ready_promise->get_future();
        auto stop_message_received_promise = std::promise< void >();
        const auto stop_message_received = stop_message_received_promise.get_future();
        auto start_message_received_promise = std::promise< void >();
        const auto start_message_received = start_message_received_promise.get_future();

        underlying_clock_type testing_clock;
        sequencer_clock_type sequencer_clock{testing_clock};
        auto midi_clock = midi::clock{sequencer_clock};
        CHECK_FALSE( midi_clock.is_running() );

        std::vector< message_t< 1 > > received_messages;
        const auto sender = message_counting_sender{received_messages};
        const auto clock_done =
            std::async( std::launch::async,
                        [&sender, &midi_clock, thread_ready = std::move( thread_ready_promise )] {
                            thread_ready->set_value();
                            midi_clock.run( sender );
                        } );
        // make sure that midi_clock.shut_down() is called before the blocking destructor of
        // clock_done is called
        const auto midi_clock_raii_shutdown =
            make_midi_clock_raii_shutdown( midi_clock, clock_done );
        thread_ready.wait();

        WHEN( "the clock is started and runs for 490ms = 0.98 beats @ 120 bpm" )
        {
            midi_clock.start();
            CHECK( midi_clock.is_running() );
            {
                std::unique_lock lock( sender.shared->message_mutex );
                sender.shared->message_received.wait(
                    lock, [&sender] { return sender.shared->start_message_count == 1; } );
            }

            auto time = testing_clock.now();
            for ( auto i = 0u; i < 49; ++i )
            {
                time += 10ms;
                testing_clock.set( time );
            }
            {
                std::unique_lock lock( sender.shared->message_mutex );
                sender.shared->message_received.wait(
                    lock, [&sender] { return sender.shared->clock_message_count == 24; } );
            }

            midi_clock.stop();
            {
                std::unique_lock lock( sender.shared->message_mutex );
                sender.shared->message_received.wait(
                    lock, [&sender] { return sender.shared->stop_message_count == 1; } );
            }

            THEN( "one start message, 24 clock messages and one stop message are sent" )
            {

                CHECK( received_messages.size() == 2u + 24u );
                CHECK( received_messages.front() == realtime_start() );
                for ( auto i = 1u; i + 1u < received_messages.size(); ++i )
                {
                    CHECK( received_messages[ i ] == realtime_clock() );
                }
                CHECK( received_messages.back() == realtime_stop() );
            }
        }
    }
}

SCENARIO( "Asynchronous control of midi-clock", "[midi_clock]" )
{
    using namespace sequencer;

    using underlying_clock_type = chrono::clock_object_adapter< std::chrono::steady_clock >;
    using sequencer_clock_type = chrono::sequencer_clock< underlying_clock_type >;

    GIVEN( "A midi clock running in a separate thread" )
    {
        auto thread_ready_promise = std::make_shared< std::promise< void > >();
        const auto thread_ready = thread_ready_promise->get_future();

        std::vector< message_t< 1 > > received_messages;
        const auto sender = message_counting_sender{received_messages};

        sequencer_clock_type sequencer_clock{underlying_clock_type{}};
        auto midi_clock = midi::clock{sequencer_clock};
        const auto clock_done =
            std::async( std::launch::async,
                        [&sender, &midi_clock, thread_ready = std::move( thread_ready_promise )] {
                            thread_ready->set_value();
                            midi_clock.run( sender );
                        } );
        // make sure that midi_clock.shut_down() is called before the blocking destructor of
        // clock_done is called
        const auto midi_clock_raii_shutdown =
            make_midi_clock_raii_shutdown( midi_clock, clock_done );
        thread_ready.wait();

        WHEN( "clock is started" )
        {
            midi_clock.start();
            {
                std::unique_lock lock( sender.shared->message_mutex );
                sender.shared->message_received.wait(
                    lock, [&sender] { return sender.shared->start_message_count == 1; } );
            }

            WHEN( "clock is stopped" )
            {
                midi_clock.stop();
                {
                    std::unique_lock lock( sender.shared->message_mutex );
                    sender.shared->message_received.wait(
                        lock, [&sender] { return sender.shared->stop_message_count == 1; } );
                }

                THEN( "start and stop messages are sent" )
                {
                    CHECK( received_messages.size() >= 2 );
                    CHECK( received_messages.front() == realtime_start() );
                    CHECK( received_messages.back() == realtime_stop() );

                    received_messages.clear();

                    WHEN( "clock is started again" )
                    {
                        midi_clock.start();
                        {
                            std::unique_lock lock( sender.shared->message_mutex );
                            sender.shared->message_received.wait( lock, [&sender] {
                                return sender.shared->continue_message_count == 1;
                            } );
                        }

                        WHEN( "clock is stopped again" )
                        {
                            midi_clock.stop();
                            {
                                std::unique_lock lock( sender.shared->message_mutex );
                                sender.shared->message_received.wait( lock, [&sender] {
                                    return sender.shared->stop_message_count == 2;
                                } );
                            }

                            THEN( "continue and stop messages are sent" )
                            {
                                CHECK( received_messages.size() >= 2 );
                                CHECK( received_messages.front() == realtime_continue() );
                                CHECK( received_messages.back() == realtime_stop() );
                            }
                        }
                    }

                    WHEN( "clock is reset and then restarted again" )
                    {
                        midi_clock.reset();
                        midi_clock.start();
                        {
                            std::unique_lock lock( sender.shared->message_mutex );
                            sender.shared->message_received.wait( lock, [&sender] {
                                return sender.shared->start_message_count == 2;
                            } );
                        }

                        WHEN( "clock is stopped again" )
                        {
                            midi_clock.stop();
                            {
                                std::unique_lock lock( sender.shared->message_mutex );
                                sender.shared->message_received.wait( lock, [&sender] {
                                    return sender.shared->stop_message_count == 2;
                                } );
                            }

                            THEN( "start and stop messages are sent" )
                            {
                                CHECK( received_messages.size() >= 2 );
                                CHECK( received_messages.front() == realtime_start() );
                                CHECK( received_messages.back() == realtime_stop() );
                            }
                        }
                    }
                }
            }
        }
    }
}

namespace
{
    struct timestamping_sender
    {
        timestamping_sender(
            std::vector< std::chrono::time_point< std::chrono::high_resolution_clock > >&
                time_points,
            std::promise< void >& stop_received )
            : time_points( time_points ), stop_received( stop_received )
        {
        }

        void operator()( message_t< 1 > message ) const
        {
            if ( message == realtime_clock() )
            {
                time_points.push_back( std::chrono::high_resolution_clock::now() );
            }
            if ( message == realtime_stop() )
            {
                stop_received.set_value();
            }
        }

        std::vector< std::chrono::time_point< std::chrono::high_resolution_clock > >& time_points;
        std::promise< void >& stop_received;
    };

    double estimate_tempo(
        const std::vector< std::chrono::time_point< std::chrono::high_resolution_clock > >&
            time_points,
        int pulses_per_quarter_note )
    {
        const auto time_span = std::chrono::duration_cast< sequencer::chrono::minutes >(
                                   time_points.back() - time_points.front() )
                                   .count();
        return static_cast< double >( time_points.size() - 1 ) /
               ( pulses_per_quarter_note * time_span );
    }
} // namespace

SCENARIO( "detect tempo of clock signals", "[midi_clock]" )
{
    using namespace sequencer;

    using underlying_clock_type = chrono::clock_object_adapter< std::chrono::steady_clock >;
    using sequencer_clock_type = chrono::sequencer_clock< underlying_clock_type >;

    GIVEN( "A midi clock running in a separate thread with a sender that stores timestamps" )
    {
        auto thread_ready_promise = std::make_shared< std::promise< void > >();
        const auto thread_ready = thread_ready_promise->get_future();

        std::vector< std::chrono::time_point< std::chrono::high_resolution_clock > > time_points;
        std::promise< void > stop_received_promise;
        const auto sender = timestamping_sender{time_points, stop_received_promise};
        const auto stop_received = stop_received_promise.get_future();

        sequencer_clock_type sequencer_clock{underlying_clock_type{}};
        auto midi_clock = midi::clock{sequencer_clock};
        const auto clock_done =
            std::async( std::launch::async,
                        [&sender, &midi_clock, thread_ready = std::move( thread_ready_promise )] {
                            thread_ready->set_value();
                            midi_clock.run( sender );
                        } );
        // make sure that midi_clock.shut_down() is called before the blocking destructor of
        // clock_done is called
        const auto midi_clock_raii_shutdown =
            make_midi_clock_raii_shutdown( midi_clock, clock_done );
        thread_ready.wait();

        WHEN( "clock is started" )
        {
            midi_clock.start();
            WHEN( "clock is stopped after running for at least 1 second" )
            {
                std::this_thread::sleep_for( std::chrono::seconds{1} );
                midi_clock.stop();
                stop_received.wait();
                THEN( "the estimated tempo from the timestamps yields 120 bpm" )
                {
                    REQUIRE( time_points.size() > 1 );
                    const auto estimated_tempo =
                        estimate_tempo( time_points, midi_clock.pulses_per_quarter_note() );
                    REQUIRE( estimated_tempo == Approx{120.0}.epsilon( 1e-3 ) );
                }
            }
        }

        WHEN( "the tempo is changed to 137 bpm" )
        {
            const auto new_tempo = 137.0_bpm;
            midi_clock.set_tempo( new_tempo );
            WHEN( "clock is started" )
            {
                midi_clock.start();
                WHEN( "clock is stopped after running for at least 1 second" )
                {
                    std::this_thread::sleep_for( std::chrono::seconds{1} );
                    midi_clock.stop();
                    stop_received.wait();
                    THEN( "the estimated tempo from the timestamps yields 137 bpm" )
                    {
                        REQUIRE( time_points.size() > 1 );
                        const auto estimated_tempo =
                            estimate_tempo( time_points, midi_clock.pulses_per_quarter_note() );
                        REQUIRE( estimated_tempo == Approx{new_tempo.to_double()}.epsilon( 1e-3 ) );
                    }
                }
            }
        }
    }
}

SCENARIO( "Change tempo of running clock", "[!mayfail][midi_clock]" )
{
    using namespace sequencer;
    using namespace std::literals::chrono_literals;

    using underlying_clock_type = chrono::steady_testing_clock<>;
    using sequencer_clock_type = chrono::sequencer_clock< const underlying_clock_type& >;

    GIVEN( "A sequencer clock with a steady testing clock" )
    {
        underlying_clock_type testing_clock;
        sequencer_clock_type sequencer_clock{testing_clock};

        GIVEN( "a started midi clock" )
        {
            auto thread_ready_promise = std::make_shared< std::promise< void > >();
            const auto thread_ready = thread_ready_promise->get_future();

            std::vector< message_t< 1 > > received_messages;
            auto sender = message_counting_sender{received_messages};
            auto midi_clock = midi::clock{sequencer_clock};
            const auto clock_done =
                std::async( std::launch::async, [&sender, &midi_clock,
                                                 thread_ready = std::move( thread_ready_promise )] {
                    thread_ready->set_value();
                    midi_clock.run( sender );
                } );
            // make sure that midi_clock.shut_down() is called before the blocking destructor of
            // clock_done is called
            const auto midi_clock_raii_shutdown =
                make_midi_clock_raii_shutdown( midi_clock, clock_done );
            thread_ready.wait();

            WHEN( "the clock is started and the sequencer clock sends time point 0" )
            {
                midi_clock.start();

                THEN( "the clock sends 1 clock message" )
                {
                    std::unique_lock lock( sender.shared->message_mutex );
                    sender.shared->message_received.wait(
                        lock, [&sender] { return sender.shared->clock_message_count == 1; } );
                    CHECK( sender.shared->clock_message_count == 1 );
                }

                WHEN( "the sequencer clock is increased 20 times by 10 ms" )
                {
                    THEN( "the clock sends 10 clock messages" )
                    {
                        {
                            std::unique_lock lock( sender.shared->message_mutex );
                            sender.shared->message_received.wait( lock, [&sender] {
                                return sender.shared->clock_message_count == 1;
                            } );
                        }
                        auto time = testing_clock.now();
                        for ( auto i = 0; i < 20; ++i )
                        {
                            time += 10ms;
                            testing_clock.set( time );
                            std::unique_lock lock( sender.shared->message_mutex );
                            sender.shared->message_received.wait( lock, [i, &sender] {
                                return sender.shared->clock_message_count ==
                                       1 + int( ( i + 1 ) / 2.083333 );
                            } );
                        }

                        REQUIRE( sender.shared->clock_message_count == 10 );

                        WHEN( "the tempo is changed to 60 bpm" )
                        {
                            midi_clock.set_tempo( 60.0_bpm );

                            REQUIRE( sender.shared->clock_message_count == 10 );
                            const auto previously_sent_messages = 10;

                            WHEN( "the sequencer clock is increased 20 times by 10 ms" )
                            {
                                THEN( "the clock sends 5 more clock messages" )
                                {
                                    auto time = underlying_clock_type::time_point{200ms};
                                    for ( auto i = 0; i < 20; ++i )
                                    {
                                        time += 10ms;
                                        testing_clock.set( time );
                                        std::unique_lock lock( sender.shared->message_mutex );
                                        // (i+1)*10ms + 2.5*10ms remaining from running clock at 120
                                        // bpm
                                        const auto new_messages =
                                            ( ( i + 1 + 2 ) * midi_clock.pulses_per_quarter_note() +
                                              midi_clock.pulses_per_quarter_note() / 2 ) /
                                            100;
                                        const auto expected_messages =
                                            previously_sent_messages + new_messages;
                                        sender.shared->message_received.wait(
                                            lock, [&sender, expected_messages] {
                                                return sender.shared->clock_message_count ==
                                                       expected_messages;
                                            } );
                                    }

                                    REQUIRE( sender.shared->clock_message_count == 15 );

                                    WHEN( "the tempo is changed to 30 bpm" )
                                    {
                                        midi_clock.set_tempo( 30.0_bpm );

                                        REQUIRE( sender.shared->clock_message_count == 15 );
                                        const auto previously_sent_messages = 15;

                                        WHEN( "the sequencer clock is increased 20 times by 10 ms" )
                                        {
                                            THEN( "the clock sends 5 more clock messages" )
                                            {
                                                auto time =
                                                    underlying_clock_type::time_point{400ms};
                                                for ( auto i = 0; i < 20; ++i )
                                                {
                                                    time += 10ms;
                                                    testing_clock.set( time );
                                                    const auto now = sequencer_clock.now();
                                                    using time_point =
                                                        std::remove_const_t< decltype( now ) >;
                                                    std::unique_lock lock(
                                                        sender.shared->message_mutex );
                                                    const auto expected_messages =
                                                        ( i < 4 )
                                                            ? previously_sent_messages
                                                            : ( i < 13 )
                                                                  ? ( previously_sent_messages + 1 )
                                                                  : ( previously_sent_messages +
                                                                      2 );
                                                    sender.shared->message_received.wait(
                                                        lock, [&sender, expected_messages] {
                                                            return sender.shared
                                                                       ->clock_message_count ==
                                                                   expected_messages;
                                                        } );
                                                }

                                                REQUIRE( sender.shared->clock_message_count == 17 );
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
