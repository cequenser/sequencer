#pragma once

#include "track.hpp"

#include <sequencer/midi/track.hpp>

#include <QDialog>

class QSpinBox;

namespace qt
{
    class normal_mode_tab_t : public QWidget
    {
        Q_OBJECT
    public:
        using pattern_t =
            std::vector< sequencer::midi::clock_to_step_t< sequencer::midi::track_t > >;
        explicit normal_mode_tab_t( pattern_t& pattern, track_t& track, QWidget* parent = nullptr );

    public slots:

        void pattern_steps_changed( int steps );

        void pattern_length_changed( int length );

        void pattern_multipler_changed( int idx );

    private:
        pattern_t& pattern_;
        track_t& track_;
        QSpinBox* step_box_;
        QSpinBox* length_box_;
        std::vector< std::pair< std::string, double > > multipliers_ = {
            {"2x", 2.0},   {"3/2x", 1.5},  {"1x", 1.0},   {"3/4x", 0.75},
            {"1/2x", 0.5}, {"1/4x", 0.25}, {"1/8x", 0.25}};
        int last_length_ = 16;
    };

    class scale_dialog_t : public QDialog
    {
        Q_OBJECT
    public:
        using pattern_t = normal_mode_tab_t::pattern_t;
        explicit scale_dialog_t( pattern_t& pattern, track_t& track, QWidget* parent = nullptr );
    };
} // namespace qt
