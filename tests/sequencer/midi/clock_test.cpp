#include "../chrono/testing_clock.hpp"

#include <sequencer/chrono/chrono_adapter.hpp>
#include <sequencer/chrono/sequencer_clock.hpp>
#include <sequencer/midi/clock.hpp>
#include <sequencer/midi/message_type.hpp>

#include <catch2/catch.hpp>

#include <chrono>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <vector>

SCENARIO( "A midi clock running for 1 beat", "[midi_clock]" )
{
    using namespace sequencer;

    using underlying_clock_type = chrono::clock_object_adapter< std::chrono::steady_clock >;
    using sequencer_clock_type = chrono::sequencer_clock< underlying_clock_type >;

    GIVEN( "a midi clock that runs for 1 beat and a test sender" )
    {
        std::vector< midi::message_type > received_messages;
        const auto sender = [&received_messages]( midi::message_type message ) {
            received_messages.push_back( message );
        };
        sequencer_clock_type sequencer_clock{underlying_clock_type{}};
        auto clock = midi::clock{sequencer_clock, sender, 0.99_beats};

        WHEN( "the clock is started" )
        {
            clock.start();
            clock.run();

            THEN( "one start message, 24 clock messages and one stop message are sent" )
            {

                CHECK( received_messages.size() == 2u + 24u );
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
    struct message_counting_sender
    {
        message_counting_sender( std::vector< sequencer::midi::message_type >& received_messages )
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
            std::condition_variable message_received;
            int start_message_count{0};
            int continue_message_count{0};
            int stop_message_count{0};
        };

        std::shared_ptr< shared_data > shared = std::make_shared< shared_data >();
    };

    template < class Clock, class Dummy >
    auto make_midi_clock_raii_shutdown( Clock& clock, Dummy& dummy )
    {
        const auto shut_down_clock_impl = [&clock]( const std::future< void >* ) {
            clock.shut_down();
        };
        return std::unique_ptr< Dummy, decltype( shut_down_clock_impl ) >( &dummy,
                                                                           shut_down_clock_impl );
    }
} // namespace

SCENARIO( "Asynchronous control of midi-clock", "[midi_clock]" )
{
    using namespace sequencer;

    using underlying_clock_type = chrono::clock_object_adapter< std::chrono::steady_clock >;
    using sequencer_clock_type = chrono::sequencer_clock< underlying_clock_type >;

    GIVEN( "A midi clock running in a separate thread" )
    {
        auto thread_ready_promise = std::make_shared< std::promise< void > >();
        const auto thread_ready = thread_ready_promise->get_future();

        std::vector< midi::message_type > received_messages;
        const auto sender = message_counting_sender{received_messages};

        sequencer_clock_type sequencer_clock{underlying_clock_type{}};
        auto midi_clock = midi::clock{sequencer_clock, sender};
        const auto clock_done = std::async(
            std::launch::async, [&midi_clock, thread_ready = std::move( thread_ready_promise )] {
                thread_ready->set_value();
                midi_clock.run();
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
                    CHECK( received_messages.front() == midi::message_type::realtime_start );
                    CHECK( received_messages.back() == midi::message_type::realtime_stop );

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
                                CHECK( received_messages.front() ==
                                       midi::message_type::realtime_continue );
                                CHECK( received_messages.back() ==
                                       midi::message_type::realtime_stop );
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

        void operator()( sequencer::midi::message_type message ) const
        {
            if ( message == sequencer::midi::message_type::realtime_clock )
            {
                time_points.push_back( std::chrono::high_resolution_clock::now() );
            }
            if ( message == sequencer::midi::message_type::realtime_stop )
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
        auto midi_clock = midi::clock{sequencer_clock, sender};
        const auto clock_done = std::async(
            std::launch::async, [&midi_clock, thread_ready = std::move( thread_ready_promise )] {
                thread_ready->set_value();
                midi_clock.run();
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

namespace
{
    struct clock_counting_sender
    {
        void operator()( sequencer::midi::message_type message ) const
        {
            if ( message == sequencer::midi::message_type::realtime_clock )
            {
                auto lock = std::unique_lock{shared->message_mutex};
                ++shared->clock_message_count;
                lock.unlock();
                shared->message_received.notify_one();
            }
        }

        struct shared_data
        {
            std::mutex message_mutex;
            std::condition_variable message_received;
            int clock_message_count{0};
        };

        std::shared_ptr< shared_data > shared = std::make_shared< shared_data >();
    };
} // namespace

SCENARIO( "Change tempo of running clock", "[midi_clock]" )
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

            auto sender = clock_counting_sender{};
            auto midi_clock = midi::clock{sequencer_clock, sender};
            const auto clock_done =
                std::async( std::launch::async,
                            [&midi_clock, thread_ready = std::move( thread_ready_promise )] {
                                thread_ready->set_value();
                                midi_clock.run();
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
