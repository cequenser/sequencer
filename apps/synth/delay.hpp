#pragma once

#include <sequencer/audio/delay.hpp>
#include <sequencer/audio/dry_wet.hpp>

#include <QGroupBox>

namespace qt
{
    class delay_t : public QGroupBox
    {
    public:
        explicit delay_t( QWidget* parent = nullptr );

        void set_delay( sequencer::audio::dry_wet_t< sequencer::audio::stereo_repeated_delay_t,
                                                     true >* delay ) noexcept;

        void set_sample_rate( int sample_rate ) noexcept;

    public slots:
        void delay_changed( int delay_in_ms );
        void delay_count_changed( int count );
        void wet_amount_changed( int amount );
        void stereo_amount_changed( int amount );
        void gain_changed( int gain );

    private:
        sequencer::audio::dry_wet_t< sequencer::audio::stereo_repeated_delay_t, true >* delay_{
            nullptr};
        int sample_rate_{44100};
    };
} // namespace qt
