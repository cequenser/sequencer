#pragma once

#include <sequencer/audio/dry_wet.hpp>
#include <sequencer/audio/oscillator.hpp>

#include <QGroupBox>

namespace qt
{
    class osc_freq_t : public QGroupBox
    {
        Q_OBJECT
    public:
        enum class mode_t
        {
            bpm_mode,
            free_mode
        };

        using oscillator_t = sequencer::audio::dry_wet_t< sequencer::audio::oscillator_t, false >;

        explicit osc_freq_t( QWidget* parent = nullptr );

        void set_oscillator( oscillator_t* oscillator );

    public slots:
        void amplitude_changed( int i );
        void wave_form_changed( int i );
        void phase_changed( int phase );
        void frequency_changed( int note );
        void note_offset_changed( int note_offset );
        void pulse_length_changed( int pulse_length );
        void set_carrier_frequency( double carrier );
        void dry_wet_ratio_changed( int ratio );

    private:
        double compute_note_offset() noexcept;

        oscillator_t* oscillator_{nullptr};
        mode_t mode_{mode_t::free_mode};
        double carrier_frequency_{440};
    };
} // namespace qt
