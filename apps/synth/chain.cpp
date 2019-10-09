#include "chain.hpp"

#include "delay.hpp"
#include "envelope.hpp"
#include "osc.hpp"
#include "osc_freq.hpp"
#include "poti.hpp"

#include <QCheckBox>
#include <QDial>
#include <QHBoxLayout>
#include <QVBoxLayout>

namespace qt
{
    chain_t::chain_t( QWidget* parent ) : QGroupBox{parent}
    {
        auto layout = new QHBoxLayout;

        auto osc = new qt::osc_t;
        osc->setTitle( "OSC" );
        layout->addWidget( osc );

        auto rm = new qt::osc_freq_t;
        rm->setTitle( "RM" );
        dynamic_cast< qt::poti_t* >( rm->layout()->itemAt( 0 )->widget() )->update( 0 );
        QObject::connect( osc, &qt::osc_t::frequency_changed, rm,
                          &qt::osc_freq_t::set_carrier_frequency );

        auto box_layout = new QVBoxLayout;
        auto check_box = new QCheckBox;
        check_box->setText( "vca" );
        QObject::connect( check_box, &QCheckBox::clicked, this,
                          &chain_t::ring_modulator_mode_changed );
        box_layout->addWidget( check_box );
        auto enabled_box = new QCheckBox;
        enabled_box->setText( "on" );
        QObject::connect( enabled_box, &QCheckBox::clicked, this,
                          &chain_t::enable_ring_modulator_mode );
        box_layout->addWidget( enabled_box );
        auto group_box = new QGroupBox;
        group_box->setLayout( box_layout );
        rm->layout()->addWidget( group_box );
        layout->addWidget( rm );

        auto fm = new qt::osc_freq_t;
        fm->setTitle( "FM" );
        dynamic_cast< qt::poti_t* >( fm->layout()->itemAt( 0 )->widget() )
            ->dial()
            .setMaximum( 10000 );
        dynamic_cast< qt::poti_t* >( fm->layout()->itemAt( 0 )->widget() )->update( 0 );
        QObject::connect( osc, &qt::osc_t::frequency_changed, fm,
                          &qt::osc_freq_t::set_carrier_frequency );
        layout->addWidget( fm );

        auto env = new qt::envelope_t;
        env->setTitle( "ENV" );
        layout->addWidget( env );

        auto filter_box = new QGroupBox;
        filter_box->setTitle( "Filter" );
        auto filter_layout = new QGridLayout;
        for ( auto i = 0; i < 2; ++i )
        {
            auto gain_poti = new qt::poti_t;
            gain_poti->dial().setMaximum( 150 );
            gain_poti->setTitle( "Gain" );
            gain_poti->update( 100 );
            QObject::connect( gain_poti, &qt::poti_t::value_changed, this,
                              i == 1 ? &chain_t::lowpass_gain_changed
                                     : &chain_t::highpass_gain_changed );
            filter_layout->addWidget( gain_poti, 0, i );
            auto filter_frequency_poti = new qt::poti_t;
            filter_frequency_poti->dial().setMinimum( 20 );
            filter_frequency_poti->dial().setMaximum( 22000 );
            filter_frequency_poti->setTitle( "Cutoff" );
            filter_frequency_poti->update( i == 0 ? 20 : 22000 );
            QObject::connect( filter_frequency_poti, &qt::poti_t::value_changed, this,
                              i == 1 ? &chain_t::lowpass_frequency_changed
                                     : &chain_t::highpass_frequency_changed );
            filter_layout->addWidget( filter_frequency_poti, 1, i );
        }
        filter_box->setLayout( filter_layout );
        layout->addWidget( filter_box );

        auto delay = new qt::delay_t;
        delay->setTitle( "delay" );
        layout->addWidget( delay );

        setLayout( layout );
    }

    void chain_t::set_chain( sequencer::backend::synth::chain_t* chain )
    {
        chain_ = chain;
        dynamic_cast< qt::osc_t* >( layout()->itemAt( 0 )->widget() )
            ->set_oscillator( &chain_->oscillator );
        dynamic_cast< qt::osc_freq_t* >( layout()->itemAt( 1 )->widget() )
            ->set_oscillator( &chain_->ring_modulation );
        dynamic_cast< qt::osc_freq_t* >( layout()->itemAt( 2 )->widget() )
            ->set_oscillator( &chain_->frequency_modulation );
        dynamic_cast< qt::envelope_t* >( layout()->itemAt( 3 )->widget() )
            ->set_envelope( &chain_->envelope );
        dynamic_cast< qt::delay_t* >( layout()->itemAt( 5 )->widget() )
            ->set_delay( &chain_->delay );
    }

    void chain_t::ring_modulator_mode_changed( bool is_vca )
    {
        chain_->vca_mode = is_vca;
    }

    void chain_t::enable_ring_modulator_mode( bool enable )
    {
        chain_->vca_enabled = enable;
    }

    void chain_t::highpass_gain_changed( int gain )
    {
        chain_->highpass_gain = gain / 100.0;
    }

    void chain_t::highpass_frequency_changed( int frequency )
    {
        chain_->highpass_frequency = frequency;
    }

    void chain_t::lowpass_gain_changed( int gain )
    {
        chain_->lowpass_gain = gain / 100.0;
    }

    void chain_t::lowpass_frequency_changed( int frequency )
    {
        chain_->lowpass_frequency = frequency;
    }
} // namespace qt
