#include "delay.hpp"

#include "poti.hpp"

#include <QDial>
#include <QVBoxLayout>

namespace qt
{
    delay_t::delay_t( QWidget* parent ) : QGroupBox( parent )
    {
        auto layout = new QGridLayout;

        auto delay_time_poti = new poti_t;
        delay_time_poti->setTitle( "Delay" );
        delay_time_poti->set_suffix( " ms" );
        delay_time_poti->dial().setMinimum( 0 );
        delay_time_poti->dial().setMaximum( 1000 );
        delay_time_poti->update( 0 );
        QObject::connect( delay_time_poti, &poti_t::value_changed, this, &delay_t::delay_changed );
        layout->addWidget( delay_time_poti, 0, 0 );

        auto delay_number_poti = new poti_t;
        delay_number_poti->setTitle( "Count" );
        delay_number_poti->dial().setMinimum( 0 );
        delay_number_poti->dial().setMaximum( 10 );
        delay_number_poti->update( 0 );
        QObject::connect( delay_number_poti, &poti_t::value_changed, this,
                          &delay_t::delay_count_changed );
        layout->addWidget( delay_number_poti, 0, 1 );

        auto gain_poti = new poti_t;
        gain_poti->setTitle( "Gain" );
        gain_poti->dial().setMinimum( 0 );
        gain_poti->dial().setMaximum( 150 );
        gain_poti->update( 50 );
        QObject::connect( gain_poti, &poti_t::value_changed, this, &delay_t::gain_changed );
        layout->addWidget( gain_poti, 1, 0 );

        auto stereo_poti = new poti_t;
        stereo_poti->setTitle( "Stereo" );
        stereo_poti->dial().setMinimum( 0 );
        stereo_poti->dial().setMaximum( 100 );
        stereo_poti->update( 50 );
        QObject::connect( stereo_poti, &poti_t::value_changed, this,
                          &delay_t::stereo_amount_changed );
        layout->addWidget( stereo_poti, 1, 1 );

        auto wet_poti = new poti_t;
        wet_poti->setTitle( "Dry/Wet" );
        wet_poti->dial().setMinimum( 0 );
        wet_poti->dial().setMaximum( 100 );
        wet_poti->update( 0 );
        QObject::connect( wet_poti, &poti_t::value_changed, this, &delay_t::wet_amount_changed );
        layout->addWidget( wet_poti, 2, 0 );

        setLayout( layout );
    }

    void delay_t::set_delay( sequencer::audio::dry_wet_t< sequencer::audio::stereo_repeated_delay_t,
                                                          true >* delay ) noexcept
    {
        delay_ = delay;
        auto grid_layout = dynamic_cast< QGridLayout* >( layout() );
        delay_changed( dynamic_cast< qt::poti_t* >( grid_layout->itemAtPosition( 0, 0 )->widget() )
                           ->dial()
                           .value() );
        delay_count_changed(
            dynamic_cast< qt::poti_t* >( grid_layout->itemAtPosition( 0, 1 )->widget() )
                ->dial()
                .value() );
        gain_changed( dynamic_cast< qt::poti_t* >( grid_layout->itemAtPosition( 1, 0 )->widget() )
                          ->dial()
                          .value() );
        stereo_amount_changed(
            dynamic_cast< qt::poti_t* >( grid_layout->itemAtPosition( 1, 1 )->widget() )
                ->dial()
                .value() );
        wet_amount_changed(
            dynamic_cast< qt::poti_t* >( grid_layout->itemAtPosition( 2, 0 )->widget() )
                ->dial()
                .value() );
    }

    void delay_t::set_sample_rate( int sample_rate ) noexcept
    {
        sample_rate_ = sample_rate;
    }

    void delay_t::delay_changed( int delay_in_ms )
    {
        if ( delay_ )
        {
            delay_->set_frame_count( ( sample_rate_ * delay_in_ms ) / 1000 );
        }
    }

    void delay_t::delay_count_changed( int count )
    {
        if ( delay_ )
        {
            delay_->set_delay_count( count );
            delay_changed(
                dynamic_cast< qt::poti_t* >( layout()->itemAt( 0 )->widget() )->dial().value() );
        }
    }

    void delay_t::wet_amount_changed( int amount )
    {
        if ( delay_ )
        {
            delay_->set_dry_wet_ratio( ( 100 - amount ) / 100.f );
        }
    }

    void delay_t::stereo_amount_changed( int amount )
    {
        if ( delay_ )
        {
            delay_->set_stereo_ratio( amount / 100.0 );
        }
    }

    void delay_t::gain_changed( int gain )
    {
        if ( delay_ )
        {
            delay_->set_gain( gain / 100.f );
        }
    }
} // namespace qt
