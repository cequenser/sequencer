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

        using track = sequencer::midi::track< 16 >;

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

        Q_INVOKABLE void set_step( int i, bool checked );

    private:
        RtMidiOut midiout_;
        sequencer::midi::step_sequencer< track, sequencer::rtmidi::message_sender > step_sequencer_;
        decltype( sequencer::rtmidi::make_clock( midiout_, step_sequencer_ ) ) clock_;
        std::future< void > clock_done_;
    };
} // namespace qml
