#pragma once

#include <sequencer/midi/message/message_type.hpp>
#include <sequencer/midi/track.hpp>

#include <vector>

class backend
{
public:
    using tracks_t = std::vector< sequencer::midi::sequencer_track_t >;
    using patterns_t = std::vector< tracks_t >;
    using banks_t = std::vector< patterns_t >;

    backend();

    void set_current_bank( int idx );
    void set_current_pattern( int idx );
    void set_current_track( int idx );
    void set_step( int step, bool value );
    bool is_step_set( int step );

    template < class Sender >
    void receive_clock_message( sequencer::midi::message_t< 1 > message, Sender sender )
    {
        send_messages( current_tracks(), message, sender );
    }

private:
    tracks_t& current_tracks();
    sequencer::midi::sequencer_track_t& current_track();

    banks_t banks_;
    banks_t::size_type current_bank_idx_ = 0;
    patterns_t::size_type current_pattern_idx_ = 0;
    tracks_t::size_type current_track_idx_ = 0;
};
