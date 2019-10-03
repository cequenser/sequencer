#include "chain.hpp"

#include "envelope.hpp"
#include "osc.hpp"
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
        auto rm = new qt::osc_t;
        dynamic_cast< qt::poti_t* >( rm->layout()->itemAt( 0 )->widget() )->update( 0 );

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
        rm->setTitle( "RM" );
        layout->addWidget( rm );
        auto fm = new qt::osc_t;
        dynamic_cast< qt::poti_t* >( fm->layout()->itemAt( 0 )->widget() )
            ->dial()
            .setMaximum( 10000 );
        dynamic_cast< qt::poti_t* >( fm->layout()->itemAt( 0 )->widget() )->update( 0 );
        fm->setTitle( "FM" );
        layout->addWidget( fm );
        auto env = new qt::envelope_t;
        env->setTitle( "ENV" );
        layout->addWidget( env );

        setLayout( layout );
    }

    void chain_t::set_chain( sequencer::backend::synth::chain_t* chain )
    {
        chain_ = chain;
        dynamic_cast< qt::osc_t* >( layout()->itemAt( 0 )->widget() )
            ->set_oscillator( &chain_->oscillator );
        dynamic_cast< qt::osc_t* >( layout()->itemAt( 1 )->widget() )
            ->set_oscillator( &chain_->ring_modulation );
        dynamic_cast< qt::osc_t* >( layout()->itemAt( 2 )->widget() )
            ->set_oscillator( &chain_->frequency_modulation );
        dynamic_cast< qt::envelope_t* >( layout()->itemAt( 3 )->widget() )
            ->set_envelope( &chain_->envelope );
    }

    void chain_t::ring_modulator_mode_changed( bool is_vca )
    {
        chain_->vca_mode_ = is_vca;
    }

    void chain_t::enable_ring_modulator_mode( bool enable )
    {
        chain_->vca_enabled_ = enable;
    }
} // namespace qt
