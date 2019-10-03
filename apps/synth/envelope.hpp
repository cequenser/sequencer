#pragma once

#include <sequencer/audio/envelope.hpp>

#include <QGroupBox>

namespace qt
{
    class envelope_t : public QGroupBox
    {
        Q_OBJECT
    public:
        explicit envelope_t( QWidget* parent = nullptr );

        void set_envelope( sequencer::audio::envelope_t* envelope );

    public slots:
        void attack_changed( int value );
        void decay_changed( int value );
        void sustain_changed( int value );

    private:
        sequencer::audio::envelope_t* envelope_{nullptr};
    };
} // namespace qt
