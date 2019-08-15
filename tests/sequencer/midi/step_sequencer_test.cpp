#include <sequencer/chrono/sequencer_clock.hpp>
#include <sequencer/chrono/testing_clock.hpp>
#include <sequencer/midi/clock.hpp>
#include <sequencer/midi/message/channel_mode.hpp>
#include <sequencer/midi/message/channel_voice.hpp>
#include <sequencer/midi/message/message_type.hpp>
#include <sequencer/midi/step_sequencer.hpp>
#include <sequencer/midi/track.hpp>

#include <catch2/catch.hpp>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <future>
#include <mutex>
#include <vector>

using sequencer::midi::step_sequencer;
using sequencer::midi::track;
using sequencer::midi::channel::mode::all_notes_off;

using underlying_clock_type = sequencer::chrono::steady_testing_clock<>;
using sequencer_clock_type = sequencer::chrono::sequencer_clock< const underlying_clock_type& >;

const auto velocity = 32;

namespace
{
    auto note_on( std::uint8_t channel, std::uint8_t note )
    {
        return sequencer::midi::message_type{
            sequencer::midi::channel::voice::note_on( channel, note, velocity )};
    }

    auto note_off( std::uint8_t channel, std::uint8_t note )
    {
        return sequencer::midi::message_type{
            sequencer::midi::channel::voice::note_off( channel, note, velocity )};
    }
} // namespace

SCENARIO( "step_sequencer_base plays 4 beats", "[step_sequencer]" )
{
    using namespace sequencer;

    GIVEN( "a step sequencer with an empty track" )
    {
        std::vector< midi::message_type > received_messages;
        const auto midi_sender = [&received_messages]( midi::message_type message ) {
            received_messages.push_back( message );
        };

        constexpr auto steps = 4u;
        const auto midi_track = track< steps >{};
        auto sequencer = step_sequencer{midi_track, midi_sender};

        WHEN( "sequencer receives start message and 96 clock messages (i.e. 4 beats @ 24 pulses "
              "per beat)" )
        {
            sequencer.update( midi::realtime::message_type::realtime_start );
            for ( auto i = 0u; i < 96; ++i )
            {
                sequencer.update( midi::realtime::message_type::realtime_clock );
            }

            THEN( "no message is sent" )
            {
                CHECK( received_messages.empty() );
            }
        }
    }

    GIVEN( "a step sequencer a track with notes on first two quarters" )
    {
        std::vector< midi::message_type > received_messages;
        const auto midi_sender = [&received_messages]( midi::message_type message ) {
            received_messages.push_back( message );
        };

        constexpr auto steps = 16u;
        auto midi_track = track< steps >{};
        const auto note_1 = 0;
        const auto note_2 = 42;
        midi_track[ 0 ] = note_1;
        midi_track[ 4 ] = note_2;
        auto sequencer = step_sequencer{midi_track, midi_sender};

        WHEN( "sequencer receives start message and one clock message" )
        {
            sequencer.update( midi::realtime::message_type::realtime_start );
            sequencer.update( midi::realtime::message_type::realtime_clock );

            THEN( "one note on message is send" )
            {
                REQUIRE( received_messages.size() == 1 );
                CHECK( received_messages.front() == note_on( 0, note_1 ) );
            }
        }

        WHEN( "sequencer receives no start message and one clock message" )
        {
            sequencer.update( midi::realtime::message_type::realtime_clock );

            THEN( "one note on message is send" )
            {
                CHECK( received_messages.empty() );
            }
        }

        WHEN( "sequencer receives start message and 24 clock messages" )
        {
            sequencer.update( midi::realtime::message_type::realtime_start );
            for ( auto i = 0u; i < 23; ++i )
            {
                sequencer.update( midi::realtime::message_type::realtime_clock );
            }

            THEN( "one note on message is send" )
            {
                REQUIRE( received_messages.size() == 1 );
                CHECK( received_messages.front() == note_on( 0, note_1 ) );
            }
            WHEN( "sequencer receives stop message one clock message" )
            {
                sequencer.update( midi::realtime::message_type::realtime_stop );
                sequencer.update( midi::realtime::message_type::realtime_clock );

                THEN( "one note on and all notes off message are send" )
                {
                    REQUIRE( received_messages.size() == 2 );
                    CHECK( received_messages[ 0 ] == note_on( 0, note_1 ) );
                    CHECK( received_messages[ 1 ] == midi::message_type{all_notes_off( 0 )} );
                }

                WHEN( "sequencer receives continue message and 25 clock messages" )
                {
                    sequencer.update( midi::realtime::message_type::realtime_continue );
                    for ( auto i = 0u; i < 25; ++i )
                    {
                        sequencer.update( midi::realtime::message_type::realtime_clock );
                    }

                    THEN( "one note off message is send" )
                    {
                        REQUIRE( received_messages.size() == 3 );
                        CHECK( received_messages[ 2 ] == note_on( 0, note_2 ) );
                    }
                }

                WHEN( "sequencer receives start message and 25 clock messages" )
                {
                    sequencer.update( midi::realtime::message_type::realtime_start );
                    for ( auto i = 0u; i < 25; ++i )
                    {
                        sequencer.update( midi::realtime::message_type::realtime_clock );
                    }

                    THEN( "one note on message is send" )
                    {
                        REQUIRE( received_messages.size() == 5 );
                        CHECK( received_messages[ 2 ] == note_on( 0, note_1 ) );

                        THEN( "one note off message is send" )
                        {
                            CHECK( received_messages[ 3 ] == note_off( 0, note_1 ) );

                            THEN( "one note on message is send" )
                            {
                                CHECK( received_messages[ 4 ] == note_on( 0, note_2 ) );
                            }
                        }
                    }
                }
            }
        }

        WHEN( "sequencer receives start message and 25 clock messages" )
        {
            sequencer.update( midi::realtime::message_type::realtime_start );
            for ( auto i = 0u; i < 25; ++i )
            {
                sequencer.update( midi::realtime::message_type::realtime_clock );
            }

            THEN( "one note on message is send" )
            {
                REQUIRE( received_messages.size() == 3 );
                CHECK( received_messages[ 0 ] == note_on( 0, note_1 ) );

                THEN( "one note off message is send" )
                {
                    CHECK( received_messages[ 1 ] == note_off( 0, note_1 ) );

                    THEN( "one note on message is send" )
                    {
                        CHECK( received_messages[ 2 ] == note_on( 0, note_2 ) );
                    }
                }
            }
        }

        WHEN( "sequencer receives start message and 96 clock messages" )
        {
            sequencer.update( midi::realtime::message_type::realtime_start );
            for ( auto i = 0u; i < 96; ++i )
            {
                sequencer.update( midi::realtime::message_type::realtime_clock );
            }

            THEN( "one note on message is send" )
            {
                REQUIRE( received_messages.size() == 3 );
                CHECK( received_messages[ 0 ] == note_on( 0, note_1 ) );

                THEN( "one note off message is send" )
                {
                    CHECK( received_messages[ 1 ] == note_off( 0, note_1 ) );

                    THEN( "one note on message is send" )
                    {
                        CHECK( received_messages[ 2 ] == note_on( 0, note_2 ) );
                    }
                }
            }
        }
    }
}

namespace
{
    template < class Clock, class Dummy >
    auto make_midi_clock_raii_shutdown( Clock& clock, Dummy& dummy )
    {
        const auto shut_down_clock_impl = [&clock]( const std::future< void >* ) {
            clock.shut_down();
        };
        return std::unique_ptr< Dummy, decltype( shut_down_clock_impl ) >( &dummy,
                                                                           shut_down_clock_impl );
    }

    struct clock_message_count_t
    {
        explicit clock_message_count_t( int count ) noexcept : count( count )
        {
        }

        clock_message_count_t& operator++() noexcept
        {
            std::unique_lock lock{mutex};
            ++count;
            lock.unlock();
            message_received.notify_one();
            return *this;
        }

        void wait_for_count( int expected_count )
        {
            std::unique_lock lock( mutex );
            message_received.wait( lock,
                                   [this, expected_count] { return count == expected_count; } );
        }

        std::mutex mutex;
        std::condition_variable message_received;
        int count{0};
    };
} // namespace

SCENARIO( "step_sequencer_base that is triggered by a midi clock plays 4 beats",
          "[step_sequencer]" )
{
    using namespace sequencer;
    using namespace std::literals::chrono_literals;

    GIVEN( "a step sequencer with an empty track" )
    {
        std::vector< midi::message_type > received_messages;
        const auto midi_sender = [&received_messages]( midi::message_type message ) {
            received_messages.push_back( message );
        };

        constexpr auto steps = 4u;
        const auto midi_track = track< steps >{};
        auto sequencer = step_sequencer{midi_track, midi_sender};
        underlying_clock_type testing_clock;
        sequencer_clock_type sequencer_clock{testing_clock};

        clock_message_count_t clock_message_count{0};
        const auto clock_sender = [&sequencer,
                                   &clock_message_count]( midi::realtime::message_type message ) {
            sequencer.update( message );
            ++clock_message_count;
        };
        auto midi_clock = midi::clock{sequencer_clock, clock_sender};

        auto thread_ready_promise = std::make_shared< std::promise< void > >();
        const auto thread_ready = thread_ready_promise->get_future();
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
        midi_clock.start();

        WHEN( "sequencer receives start message and 96 clock messages (i.e. 4 beats @ 24 pulses "
              "per beat)" )
        {
            testing_clock.set( testing_clock.now() + 1999ms );
            clock_message_count.wait_for_count( 97 );

            THEN( "no message is sent" )
            {
                CHECK( received_messages.empty() );
            }
        }
    }

    GIVEN( "a step sequencer a track with notes on first two quarters" )
    {
        std::vector< midi::message_type > received_messages;
        const auto midi_sender = [&received_messages]( midi::message_type message ) {
            received_messages.push_back( message );
        };

        constexpr auto steps = 16u;
        auto midi_track = track< steps >{};
        const auto note_1 = 0;
        const auto note_2 = 42;
        midi_track[ 0 ] = note_1;
        midi_track[ 4 ] = note_2;
        auto sequencer = step_sequencer{midi_track, midi_sender};
        underlying_clock_type testing_clock;
        sequencer_clock_type sequencer_clock{testing_clock};

        clock_message_count_t clock_message_count{0};
        const auto clock_sender = [&sequencer,
                                   &clock_message_count]( midi::realtime::message_type message ) {
            sequencer.update( message );
            ++clock_message_count;
        };
        auto midi_clock = midi::clock{sequencer_clock, clock_sender};

        auto thread_ready_promise = std::make_shared< std::promise< void > >();
        const auto thread_ready = thread_ready_promise->get_future();
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
        midi_clock.start();

        WHEN( "testing clock runs for 10 ms" )
        {
            testing_clock.set( testing_clock.now() + 10ms );
            clock_message_count.wait_for_count( 2 );

            THEN( "one note on message is send" )
            {
                REQUIRE( received_messages.size() == 1 );
                CHECK( received_messages.front() == note_on( 0, note_1 ) );
            }
        }

        WHEN( "testing clock runs for 500 ms" )
        {
            testing_clock.set( testing_clock.now() + 500ms );
            clock_message_count.wait_for_count( 26 );

            THEN( "one note on message is send" )
            {
                REQUIRE( received_messages.size() == 3 );
                CHECK( received_messages[ 0 ] == note_on( 0, note_1 ) );

                THEN( "one note off message is send" )
                {
                    CHECK( received_messages[ 1 ] == note_off( 0, note_1 ) );

                    THEN( "one note on message is send" )
                    {
                        CHECK( received_messages[ 2 ] == note_on( 0, note_2 ) );
                    }
                }
            }
        }

        WHEN( "testing clock runs for 499 ms" )
        {
            testing_clock.set( testing_clock.now() + 499ms );
            clock_message_count.wait_for_count( 25 );

            THEN( "one note on message is send" )
            {
                REQUIRE( received_messages.size() == 1 );
                CHECK( received_messages.front() == note_on( 0, note_1 ) );
            }

            WHEN( "midi clock is stopped" )
            {
                midi_clock.stop();
                clock_message_count.wait_for_count( 26 );

                THEN( "all notes off message is send" )
                {
                    REQUIRE( received_messages.size() == 2 );
                    CHECK( received_messages[ 1 ] == midi::message_type{all_notes_off( 0 )} );
                }

                WHEN( "midi clock is started again and testing clock runs for 500 ms" )
                {
                    midi_clock.start();
                    testing_clock.set( testing_clock.now() + 500ms );
                    clock_message_count.wait_for_count( 51 );

                    THEN( "one note on message is send" )
                    {
                        REQUIRE( received_messages.size() == 3 );
                        CHECK( received_messages[ 2 ] == note_on( 0, note_2 ) );
                    }
                }

                WHEN(
                    "midi clock is reset and then started again and testing clock runs for 500 ms" )
                {
                    midi_clock.reset();
                    midi_clock.start();
                    testing_clock.set( testing_clock.now() + 500ms );
                    clock_message_count.wait_for_count( 51 );

                    THEN( "one note on message is send" )
                    {
                        REQUIRE( received_messages.size() == 5 );
                        CHECK( received_messages[ 2 ] == note_on( 0, note_1 ) );

                        THEN( "one note off message is send" )
                        {
                            CHECK( received_messages[ 3 ] == note_off( 0, note_1 ) );

                            THEN( "one note on message is send" )
                            {
                                CHECK( received_messages[ 4 ] == note_on( 0, note_2 ) );
                            }
                        }
                    }
                }
            }
        }
    }
}

SCENARIO( "step_sequencer_base sends notes to correct channels", "[step_sequencer]" )
{
    using namespace sequencer;

    GIVEN( "a step sequencer with an empty track" )
    {
        std::vector< midi::message_type > received_messages;
        const auto midi_sender = [&received_messages]( midi::message_type message ) {
            received_messages.push_back( message );
        };

        constexpr auto steps = 16u;
        auto midi_track = track< steps >{};
        const auto note_1 = 0;
        midi_track[ 0 ] = note_1;
        auto sequencer = step_sequencer{midi_track, midi_sender};

        WHEN( "sequencer receives start message and 1 clock message" )
        {
            sequencer.update( midi::realtime::message_type::realtime_start );
            sequencer.update( midi::realtime::message_type::realtime_clock );

            THEN( "one note on message is send for channel 0" )
            {
                REQUIRE( received_messages.size() == 1 );
                CHECK( received_messages.front() == note_on( 0, note_1 ) );
            }
        }

        WHEN( "channel is set to 1" )
        {
            const auto new_channel = 1;
            sequencer.set_channel( new_channel );
            WHEN( "sequencer receives start message and 1 clock message" )
            {
                sequencer.update( midi::realtime::message_type::realtime_start );
                sequencer.update( midi::realtime::message_type::realtime_clock );

                THEN( "one note on message is send for channel 1" )
                {
                    REQUIRE( received_messages.size() == 1 );
                    CHECK( received_messages.front() == note_on( new_channel, note_1 ) );
                }
            }
        }
    }
}
