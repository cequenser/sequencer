#pragma once

#include <sequencer/midi/message/message_type.hpp>
#include <sequencer/midi/track.hpp>

#include <vector>

enum class backend_mode
{
    play,
    recording,
    mute
};

class backend
{
public:
    using tracks_t = std::vector< sequencer::midi::sequencer_track_t >;
    using patterns_t = std::vector< tracks_t >;
    using banks_t = std::vector< patterns_t >;

    backend();

    void set_current_bank( int idx ) noexcept;
    void set_current_pattern( int idx ) noexcept;
    void set_current_track( int idx ) noexcept;
    void set_step( int step, bool value ) noexcept;
    bool is_step_set( int step ) const noexcept;
    void set_mode( backend_mode mode ) noexcept;
    backend_mode mode() const noexcept;

    template < class Sender >
    void receive_clock_message( sequencer::midi::message_t< 1 > message, Sender sender )
    {
        send_messages( current_pattern(), message, sender );
    }

    sequencer::midi::sequencer_track_t& current_track() noexcept;
    const sequencer::midi::sequencer_track_t& current_track() const noexcept;

    tracks_t& current_pattern() noexcept;
    const tracks_t& current_pattern() const noexcept;

private:
    banks_t banks_;
    banks_t::size_type current_bank_idx_ = 0;
    patterns_t::size_type current_pattern_idx_ = 0;
    tracks_t::size_type current_track_idx_ = 0;
    backend_mode mode_ = backend_mode::play;
};
