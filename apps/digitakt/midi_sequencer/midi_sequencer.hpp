#pragma once

#include <sequencer/rtmidi/util.hpp>

#include <QMainWindow>
#include <QString>
#include <RtMidi.h>
#include <future>

namespace Ui
{
    class midi_sequencer;
}

namespace qt
{
    class dial_line_edit_sync;
}

class midi_sequencer : public QMainWindow
{
    Q_OBJECT

public:
    explicit midi_sequencer( QWidget* parent = nullptr );
    ~midi_sequencer();

public slots:
    void start_clock();
    void stop_clock();
    void set_clock_bpm( double value );
    void select_port( int idx );

private:
    void scan_available_ports();

    RtMidiOut midiout_;
    decltype( sequencer::rtmidi::make_clock() ) clock_;
    std::future< void > clock_done_;
    Ui::midi_sequencer* ui;
    int old_clock_dial_value_ = 1200;

    qt::dial_line_edit_sync* clock_ui_sync;
};
