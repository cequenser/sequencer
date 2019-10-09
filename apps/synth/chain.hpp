#pragma once

#include <sequencer/backend/synth.hpp>

#include <QGroupBox>

namespace qt
{
    class chain_t : public QGroupBox
    {
        Q_OBJECT
    public:
        explicit chain_t( QWidget* parent = nullptr );

        void set_chain( sequencer::backend::synth::chain_t* chain );

    public slots:
        void ring_modulator_mode_changed( bool is_vca );
        void enable_ring_modulator_mode( bool enable );
        void highpass_gain_changed( int gain );
        void highpass_frequency_changed( int frequency );
        void lowpass_gain_changed( int gain );
        void lowpass_frequency_changed( int frequency );

    private:
        sequencer::backend::synth::chain_t* chain_{nullptr};
    };
} // namespace qt
