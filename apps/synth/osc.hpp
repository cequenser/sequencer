#pragma once

#include <sequencer/audio/oscillator.hpp>

#include <QGroupBox>

namespace qt
{
    class osc_t : public QGroupBox
    {
        Q_OBJECT
    public:
        explicit osc_t( QWidget* parent = nullptr );

        void set_oscillator( sequencer::audio::oscillator_t* oscillator );

    public slots:
        void amplitude_changed( int i );
        void wave_form_changed( int i );
        void phase_changed( int phase );
        void note_changed( int note );
        void octave_changed( int octave );
        void pulse_length_changed( int pulse_length );

    private:
        void update_frequency();

        sequencer::audio::oscillator_t* oscillator_{nullptr};
        int note_idx_{0};
        int octave_offset_{0};
    };
} // namespace qt
