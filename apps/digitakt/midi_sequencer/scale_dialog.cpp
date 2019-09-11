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
    normal_mode_tab_t::normal_mode_tab_t( pattern_t& pattern, track_t& track, QWidget* parent )
        : QWidget{parent}, pattern_{pattern}, track_{track}
    {
        const auto steps = pattern.front().steps();
        auto layout = new QHBoxLayout;
        step_box_ = new QSpinBox;
        step_box_->setMaximum( 4 * track.fixed_size() );
        step_box_->setMinimum( 1 );
        step_box_->setValue( steps );
        step_box_->setMaximumWidth( 40 );
        QObject::connect( step_box_, qOverload< int >( &QSpinBox::valueChanged ), this,
                          &normal_mode_tab_t::pattern_steps_changed );
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
                          &normal_mode_tab_t::pattern_length_changed );
        layout->addWidget( length_box_ );
        auto combo_box = new QComboBox;
        std::for_each( begin( multipliers_ ), end( multipliers_ ),
                       [&combo_box]( const auto& multiplier ) {
                           combo_box->addItem( multiplier.first.c_str() );
                       } );
        combo_box->setCurrentIndex( 2 );
        QObject::connect( combo_box, qOverload< int >( &QComboBox::currentIndexChanged ), this,
                          &normal_mode_tab_t::pattern_multipler_changed );
        layout->addWidget( combo_box );

        setLayout( layout );
    }

    void normal_mode_tab_t::pattern_steps_changed( int steps )
    {
        for ( auto& track : pattern_ )
        {
            track.set_steps( steps, track_.fixed_size() );
        }
        track_.update_on_scale_change( steps );
        signal_blocker_t signal_blocker{*length_box_};
        length_box_->setValue( track_.page_count() * track_.fixed_size() );
        last_length_ = length_box_->value();
    }

    void normal_mode_tab_t::pattern_length_changed( int length )
    {
        if ( length > last_length_ && track_.size() < last_length_ )
        {
            step_box_->setValue( last_length_ );
            return;
        }

        step_box_->setValue( length );
    }

    void normal_mode_tab_t::pattern_multipler_changed( int idx )
    {
        for ( auto& track : pattern_ )
        {
            track.set_pulses_per_quarter_note( std::size_t(
                sequencer::midi::default_pulses_per_quarter_note / multipliers_[ idx ].second +
                1e-15 ) );
        }
    }

    scale_dialog_t::scale_dialog_t( pattern_t& pattern, track_t& track, QWidget* parent )
        : QDialog( parent )
    {
        auto layout = new QHBoxLayout;

        auto tabs = new QTabWidget;
        tabs->addTab( new normal_mode_tab_t( pattern, track ), tr( "Normal" ) );
        layout->addWidget( tabs );

        setLayout( layout );
    }
} // namespace qt
