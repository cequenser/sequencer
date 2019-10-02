#pragma once

#include "track.hpp"

#include <sequencer/midi/pattern.hpp>
#include <sequencer/midi/track.hpp>

#include <QDialog>

class QSpinBox;

namespace qt
{
    class scale_dialog_base_tab_t : public QWidget
    {
        Q_OBJECT
    public:
        explicit scale_dialog_base_tab_t( int steps, track_t& track, QWidget* parent = nullptr );

    public slots:
        void steps_changed( int steps );

        void length_changed( int length );

        void multipler_changed( int idx );

    protected:
        int fixed_size() const noexcept;
        double multiplier( int idx ) const noexcept;
        virtual void on_steps_changed( int steps ) = 0;
        virtual void on_multiplier_changed( int idx ) = 0;

    private:
        track_t& track_;
        QSpinBox* step_box_;
        QSpinBox* length_box_;
        std::vector< std::pair< std::string, double > > multipliers_ = {
            {"2x", 2.0},   {"3/2x", 1.5},  {"1x", 1.0},   {"3/4x", 0.75},
            {"1/2x", 0.5}, {"1/4x", 0.25}, {"1/8x", 0.25}};
        int last_length_ = 16;
    };

    class normal_mode_tab_t : public scale_dialog_base_tab_t
    {
        Q_OBJECT
    public:
        using midi_track_t =
            sequencer::midi::track_t< sequencer::midi::step_t,
                                      sequencer::backend::digitakt::track_parameter_t >;
        using pattern_t = sequencer::midi::pattern_t< midi_track_t >;

        explicit normal_mode_tab_t( pattern_t& pattern, track_t& track, QWidget* parent = nullptr );

    private:
        void on_steps_changed( int steps ) override;
        void on_multiplier_changed( int idx ) override;

        pattern_t& pattern_;
    };

    class advanced_mode_tab_t : public scale_dialog_base_tab_t
    {
        Q_OBJECT
    public:
        using pattern_t = normal_mode_tab_t::pattern_t;

        explicit advanced_mode_tab_t( pattern_t& pattern,
                                      normal_mode_tab_t::midi_track_t& midi_track, track_t& track,
                                      QWidget* parent = nullptr );

    public slots:
        void loop_length_changed( int length );

    private:
        void on_steps_changed( int steps ) override;
        void on_multiplier_changed( int idx ) override;

        pattern_t& pattern_;
        normal_mode_tab_t::midi_track_t& midi_track_;
    };

    class scale_dialog_t : public QDialog
    {
        Q_OBJECT
    public:
        using pattern_t = normal_mode_tab_t::pattern_t;

        explicit scale_dialog_t( pattern_t& pattern,
                                 normal_mode_tab_t::midi_track_t& current_midi_track,
                                 track_t& track, QWidget* parent = nullptr );
    };
} // namespace qt
