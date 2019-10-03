#ifndef SYNTH_HPP
#define SYNTH_HPP

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
    explicit synth( QWidget* parent = nullptr );
    ~synth();

public slots:
    void play();
    void trig();
    void audio_device_changed( int device_id );

private:
    Ui::synth* ui;
    std::atomic_bool play_{false};
    sequencer::backend::synth::backend_t backend_{};
};

#endif // SYNTH_HPP
