#include "scale_dialog.hpp"

#include "signal_blocker.hpp"
#include "util.hpp"

#include <QComboBox>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QTabWidget>
#include <algorithm>
#include <string>
#include <tuple>
#include <vector>

namespace qt
{
    scale_dialog_base_tab_t::scale_dialog_base_tab_t( int steps, track_t& track, QWidget* parent )
        : QWidget{parent}, track_{track}
    {
        auto layout = new QHBoxLayout;
        step_box_ = new QSpinBox;
        step_box_->setMaximum( 4 * track.fixed_size() );
        step_box_->setMinimum( 1 );
        step_box_->setValue( steps );
        step_box_->setMaximumWidth( 40 );
        step_box_->setToolTip( "number of steps" );
        QObject::connect( step_box_, qOverload< int >( &QSpinBox::valueChanged ), this,
                          &scale_dialog_base_tab_t::steps_changed );
        layout->addWidget( step_box_ );
        length_box_ = new QSpinBox;
        length_box_->setMaximum( 4 * track.fixed_size() );
        length_box_->setMinimum( track.fixed_size() );
        length_box_->setSingleStep( track.fixed_size() );
        length_box_->setValue(
            ( track.size() / track.fixed_size() + bool( track.size() % track.fixed_size() ) ) *
            track_.fixed_size() );
        length_box_->setMaximumWidth( 40 );
        QObject::connect( length_box_, qOverload< int >( &QSpinBox::valueChanged ), this,
                          &scale_dialog_base_tab_t::length_changed );
        layout->addWidget( length_box_ );

        auto combo_box = new QComboBox;
        std::for_each( begin( multipliers_ ), end( multipliers_ ),
                       [&combo_box]( const auto& multiplier ) {
                           combo_box->addItem( multiplier.first.c_str() );
                       } );
        combo_box->setCurrentIndex( 2 );
        combo_box->setToolTip( "speed multiplier" );
        QObject::connect( combo_box, qOverload< int >( &QComboBox::currentIndexChanged ), this,
                          &scale_dialog_base_tab_t::multipler_changed );
        layout->addWidget( combo_box );

        setLayout( layout );
    }

    void scale_dialog_base_tab_t::steps_changed( int steps )
    {
        on_steps_changed( steps );
        track_.update_on_scale_change( steps );
        signal_blocker_t signal_blocker{*length_box_};
        length_box_->setValue( track_.page_count() * track_.fixed_size() );
        last_length_ = length_box_->value();
    }

    void scale_dialog_base_tab_t::length_changed( int length )
    {
        if ( length > last_length_ && track_.size() < last_length_ )
        {
            step_box_->setValue( last_length_ );
            return;
        }

        step_box_->setValue( length );
    }

    void scale_dialog_base_tab_t::multipler_changed( int idx )
    {
        on_multiplier_changed( idx );
    }

    int scale_dialog_base_tab_t::fixed_size() const noexcept
    {
        return track_.fixed_size();
    }

    double scale_dialog_base_tab_t::multiplier( int idx ) const noexcept
    {
        return multipliers_[ std::size_t( idx ) ].second;
    }

    normal_mode_tab_t::normal_mode_tab_t( pattern_t& pattern, track_t& track, QWidget* parent )
        : scale_dialog_base_tab_t( pattern[ 0 ].steps(), track, parent ), pattern_( pattern )
    {
    }

    void normal_mode_tab_t::on_steps_changed( int steps )
    {
        pattern_.set_steps( steps, fixed_size() );
    }

    void normal_mode_tab_t::on_multiplier_changed( int idx )
    {
        pattern_.set_pulses_per_quarter_note( std::size_t(
            sequencer::midi::default_pulses_per_quarter_note / multiplier( idx ) + 1e-15 ) );
    }

    advanced_mode_tab_t::advanced_mode_tab_t( pattern_t& pattern,
                                              sequencer::midi::sequencer_track_t& midi_track,
                                              track_t& track, QWidget* parent )
        : scale_dialog_base_tab_t( midi_track.steps(), track, parent ), pattern_( pattern ),
          midi_track_( midi_track )
    {

        auto length_box = new QSpinBox;
        length_box->setMinimum( 0 );
        length_box->setMaximum( 1024 );
        length_box->setValue( 16 );
        length_box->setToolTip( "number of steps until all tracks are restarted" );
        layout()->addWidget( length_box );
        connect( length_box, qOverload< int >( &QSpinBox::valueChanged ), this,
                 &advanced_mode_tab_t::loop_length_changed );
    }

    void advanced_mode_tab_t::loop_length_changed( int length )
    {
        pattern_.set_loop_length( std::size_t( length ) );
    }

    void advanced_mode_tab_t::on_steps_changed( int steps )
    {
        midi_track_.set_steps( std::size_t( steps ), std::size_t( fixed_size() ) );
    }

    void advanced_mode_tab_t::on_multiplier_changed( int idx )
    {
        pattern_.set_pulses_per_quarter_note( std::size_t(
            sequencer::midi::default_pulses_per_quarter_note / multiplier( idx ) + 1e-15 ) );
    }

    scale_dialog_t::scale_dialog_t(
        pattern_t& pattern,
        sequencer::midi::clock_to_step_t< sequencer::midi::track_t >& current_midi_track,
        track_t& track, QWidget* parent )
        : QDialog( parent )
    {
        auto layout = new QHBoxLayout;

        auto tabs = new QTabWidget;
        tabs->addTab( new normal_mode_tab_t( pattern, track ), tr( "Normal" ) );
        tabs->addTab( new advanced_mode_tab_t( pattern, current_midi_track, track ),
                      tr( "Advanced" ) );
        layout->addWidget( tabs );

        setLayout( layout );
    }
} // namespace qt
