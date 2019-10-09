#ifndef SYNTH_HPP
#define SYNTH_HPP

#include <sequencer/backend/rtmidi.hpp>
#include <sequencer/backend/synth.hpp>

#include <QMainWindow>
#include <atomic>

namespace Ui
{
    class synth;
}

class synth : public QMainWindow
{
    Q_OBJECT

public:
    using backend_t = sequencer::backend::synth::backend_t< sequencer::backend::rtmidi_receiver_t >;

    explicit synth( QWidget* parent = nullptr,
                    void ( *callback )( double, std::vector< unsigned char >*, void* ) = nullptr );
    ~synth();

    backend_t& backend() noexcept
    {
        return backend_;
    }

public slots:
    void play();
    void trig();
    void audio_device_changed( int device_id );
    void midi_port_changed( int port_id );

private:
    Ui::synth* ui;
    std::atomic_bool play_{false};
    backend_t backend_;
};

#endif // SYNTH_HPP
