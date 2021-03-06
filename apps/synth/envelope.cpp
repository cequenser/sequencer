#include "envelope.hpp"

#include "poti.hpp"

#include <QDial>
#include <QVBoxLayout>

namespace qt
{
    constexpr auto max_values = 1001u;
    constexpr auto max_sustain_value = 2000u;

    envelope_t::envelope_t( QWidget* parent ) : QGroupBox( parent )
    {
        auto layout = new QVBoxLayout;

        auto attack_poti = new poti_t;
        attack_poti->setTitle( "Attack" );
        attack_poti->dial().setMinimum( 0 );
        attack_poti->dial().setMaximum( max_values - 1 );
        QObject::connect( attack_poti, &poti_t::value_changed, this, &envelope_t::attack_changed );
        attack_poti->update( 0 );
        layout->addWidget( attack_poti );

        auto decay_poti = new poti_t;
        decay_poti->setTitle( "Decay" );
        decay_poti->dial().setMinimum( 0 );
        decay_poti->dial().setMaximum( max_values - 1 );
        QObject::connect( decay_poti, &poti_t::value_changed, this, &envelope_t::decay_changed );
        decay_poti->update( 0 );
        layout->addWidget( decay_poti );

        auto sustain_poti = new poti_t;
        sustain_poti->setTitle( "Sustain" );
        sustain_poti->dial().setMinimum( 0 );
        sustain_poti->dial().setMaximum( max_sustain_value );
        QObject::connect( sustain_poti, &poti_t::value_changed, this,
                          &envelope_t::sustain_changed );
        sustain_poti->update( max_sustain_value );
        layout->addWidget( sustain_poti );

        setLayout( layout );
    }

    void envelope_t::set_envelope( sequencer::audio::envelope_t* envelope )
    {
        envelope_ = envelope;

        attack_changed(
            dynamic_cast< poti_t* >( layout()->itemAt( 0 )->widget() )->dial().value() );
        decay_changed( dynamic_cast< poti_t* >( layout()->itemAt( 1 )->widget() )->dial().value() );
        sustain_changed(
            dynamic_cast< poti_t* >( layout()->itemAt( 2 )->widget() )->dial().value() );
    }

    void envelope_t::attack_changed( int value )
    {
        if ( envelope_ )
        {
            envelope_->set_attack( value / 1000.0 );
        }
    }

    void envelope_t::decay_changed( int value )
    {
        if ( envelope_ )
        {
            envelope_->set_decay( value / 1000.0 );
        }
    }

    void envelope_t::sustain_changed( int value )
    {
        if ( envelope_ )
        {
            if ( value == max_sustain_value )
            {
                envelope_->set_sustain( std::numeric_limits< double >::max() );
                return;
            }
            envelope_->set_sustain( value / 1000.0 );
        }
    }
} // namespace qt
