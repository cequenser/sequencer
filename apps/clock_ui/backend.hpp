#pragma once

#include <sequencer/midi/step_sequencer.hpp>
#include <sequencer/midi/track.hpp>
#include <sequencer/rtmidi/util.hpp>

#include <QObject>
#include <QString>
#include <RtMidi.h>
#include <future>

namespace qml
{
    class backend : public QObject
    {
        Q_OBJECT

        static constexpr auto number_of_steps = 16;
        static constexpr auto number_of_tracks = 8;
        using tracks_t =
            decltype( sequencer::midi::make_tracks< number_of_steps, number_of_tracks >() );

    public:
        backend();

        ~backend();

        Q_INVOKABLE void start_clock();

        Q_INVOKABLE void stop_clock();

        Q_INVOKABLE void reset_clock();

        Q_INVOKABLE void set_clock_tempo( int bpm );

        Q_INVOKABLE void set_song_position_pointer( int song_position_in_16th_notes );

        Q_INVOKABLE QString available_ports();

        Q_INVOKABLE bool open_port( unsigned id );

        Q_INVOKABLE void set_step( int step, bool checked );

        Q_INVOKABLE bool is_step_set( int step ) const;

        Q_INVOKABLE void set_current_track( int track );

        Q_INVOKABLE int min_note() const;

        Q_INVOKABLE int max_note() const;

        Q_INVOKABLE QString note_to_string( int note ) const;

        Q_INVOKABLE QString notes_to_string() const;

        Q_INVOKABLE void set_note_of_current_track( int note );

    private:
        RtMidiOut midiout_;
        tracks_t tracks_{};
        sequencer::midi::step_sequencer< tracks_t > step_sequencer_;
        decltype( sequencer::rtmidi::make_clock() ) clock_;
        std::future< void > clock_done_;
        std::uint8_t current_track_{0};
        std::array< sequencer::midi::note_t, number_of_tracks > track_notes_;
    };
} // namespace qml
