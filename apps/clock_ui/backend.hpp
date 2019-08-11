#pragma once

#include <sequencer/rtmidi/util.hpp>

#include <QObject>
#include <RtMidi.h>
#include <future>
#include <optional>

namespace qml
{
    class backend : public QObject
    {
        Q_OBJECT
    public:
        backend();

        ~backend();

        Q_INVOKABLE void start_clock();

        Q_INVOKABLE void stop_clock();

        Q_INVOKABLE void reset_clock();

        Q_INVOKABLE void set_clock_tempo( int bpm );

        Q_INVOKABLE void set_song_position_pointer( int song_position_in_16th_notes );

    private:
        std::optional< RtMidiOut > midiout_;
        decltype( sequencer::rtmidi::make_clock( *midiout_ ) ) clock_;
        std::future< void > clock_done_;
    };
} // namespace qml
