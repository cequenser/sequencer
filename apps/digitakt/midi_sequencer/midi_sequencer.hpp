#pragma once

#include <sequencer/backend/clock.hpp>
#include <sequencer/backend/digitakt.hpp>
#include <sequencer/backend/rtmidi.hpp>
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

    using backend_t = sequencer::backend::digitakt::backend_t<
        sequencer::backend::clock_t, sequencer::backend::digitakt::backend_impl,
        sequencer::backend::rtmidi_sender_t, sequencer::backend::digitakt::rtmidi_receiver_t,
        sequencer::backend::rtmidi_sender_t >;

public:
    explicit midi_sequencer( QWidget* parent = nullptr,
                             void ( *callback )( double, std::vector< unsigned char >*,
                                                 void* ) = nullptr );
    ~midi_sequencer();

    backend_t& backend() noexcept
    {
        return backend_;
    }

public slots:
    void start_clock();
    void stop_clock();
    void set_clock_bpm( int value );
    void select_clock_port( int idx );
    void select_output_port( int idx );
    void set_slave_mode( bool slave );

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
    void update_clock_ports();
    void update_sequencer_steps();
    void update_buttons();
    void update_potis();

    backend_t backend_;
    Ui::midi_sequencer* ui;
    int old_clock_dial_value_ = 1200;
    int page_ = 0;
    bool is_clock_slave_{false};
};
