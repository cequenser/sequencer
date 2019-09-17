#pragma once

#include <sequencer/backend/digitakt.hpp>
#include <sequencer/rtmidi/util.hpp>

#include <QMainWindow>
#include <QString>
#include <RtMidi.h>
#include <future>

namespace Ui
{
    class midi_sequencer;
}

class midi_sequencer : public QMainWindow
{
    Q_OBJECT

    static constexpr auto number_of_control_potis = 8u;

public:
    explicit midi_sequencer( QWidget* parent = nullptr );
    ~midi_sequencer();

public slots:
    void start_clock();
    void stop_clock();
    void set_clock_bpm( int value );
    void select_port( int idx );

    void change_bank();
    void change_pattern();
    void change_track();
    void sequencer_step_changed( int idx );

    void trig_selected();
    void source_selected();
    void filter_selected();
    void amp_selected();
    void lfo_selected();

    void control_poti_changed( int id, int value );

private:
    void scan_available_ports();
    void update_sequencer_steps();
    void update_buttons();
    void update_potis();

    RtMidiOut midiout_;
    sequencer::backend::digitakt backend_;
    decltype( sequencer::rtmidi::make_clock() ) clock_;
    std::future< void > clock_done_;
    Ui::midi_sequencer* ui;
    int old_clock_dial_value_ = 1200;
    int page_ = 0;
};
