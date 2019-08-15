#pragma once

#include <memory>

namespace sequencer::midi
{
    template < class Clock, class Dummy >
    auto make_midi_clock_raii_shutdown( Clock& clock, Dummy& dummy )
    {
        const auto shut_down_clock_impl = [&clock]( const Dummy* ) { clock.shut_down(); };
        return std::unique_ptr< Dummy, decltype( shut_down_clock_impl ) >( &dummy,
                                                                           shut_down_clock_impl );
    }
} // namespace sequencer::midi
