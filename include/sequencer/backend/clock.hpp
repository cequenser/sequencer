#pragma once

#include <sequencer/beats_per_minute.hpp>
#include <sequencer/midi/clock.hpp>

#include <future>
#include <type_traits>

namespace sequencer::backend
{
    class clock_t
    {
    public:
        template < class Sender >
        explicit clock_t( Sender sender )
            : clock_done_{start_clock_in_thread(
                  clock_, [sender = std::move( sender )]( auto message ) { sender( message ); } )}
        {
        }

        ~clock_t()
        {
            clock_.shut_down();
        }

        void start_clock()
        {
            clock_.start();
        }

        void stop_clock()
        {
            if ( !clock_.is_running() )
            {
                clock_.reset();
                return;
            }

            clock_.stop();
        }

        void set_clock_bpm( int value )
        {
            clock_.set_tempo( beats_per_minute( value ) );
        }

    private:
        decltype( midi::make_clock() ) clock_ = midi::make_clock();
        std::future< void > clock_done_;
    };
} // namespace sequencer::backend
