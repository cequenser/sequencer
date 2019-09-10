#include "backend.hpp"

#include <cassert>

using sequencer::midi::make_tracks;
using sequencer::midi::sequencer_track_t;

backend::backend() : banks_( 16, patterns_t( 16, make_tracks( 16, 16 ) ) )
{
}

void backend::set_current_bank( int idx )
{
    assert( idx >= 0 );
    current_bank_idx_ = banks_t::size_type( idx );
}

void backend::set_current_pattern( int idx )
{
    assert( idx >= 0 );
    current_pattern_idx_ = patterns_t::size_type( idx );
}

void backend::set_current_track( int idx )
{
    assert( idx >= 0 );
    current_track_idx_ = tracks_t::size_type( idx );
}

void backend::set_step( int step, bool value )
{
    const auto note = value ? sequencer::midi::note_t{48} : sequencer::midi::no_note();
    current_track()[ sequencer_track_t::size_type( step ) ] = note;
}

bool backend::is_step_set( int step )
{
    return current_track()[ sequencer_track_t::size_type( step ) ] != sequencer::midi::no_note();
}

backend::tracks_t& backend::current_tracks()
{
    return banks_[ current_bank_idx_ ][ current_pattern_idx_ ];
}

sequencer::midi::sequencer_track_t& backend::current_track()
{
    return current_tracks()[ current_track_idx_ ];
}
