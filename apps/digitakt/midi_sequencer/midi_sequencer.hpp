#pragma once

#include <sequencer/rtmidi/util.hpp>

#include <QMainWindow>
#include <QString>
#include <RtMidi.h>
#include <backend.hpp>
#include <future>

namespace Ui
{
    class midi_sequencer;
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

    void change_bank();
    void change_pattern();
    void change_track();
    void sequencer_step_changed( int idx );

private:
    void scan_available_ports();
    void update_sequencer_steps();
    void enable_all_buttons();
    bool use_secondary_function();
    void reset_mode();

    RtMidiOut midiout_;
    backend backend_;
    decltype( sequencer::rtmidi::make_clock() ) clock_;
    std::future< void > clock_done_;
    Ui::midi_sequencer* ui;
    int old_clock_dial_value_ = 1200;

    //    qt::dial_line_edit_sync* clock_ui_sync;
};
