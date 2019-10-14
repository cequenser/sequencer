#include "osc_freq.hpp"

#include "poti.hpp"

#include <sequencer/audio/note.hpp>

#include <QDial>
#include <QGridLayout>
#include <string>
#include <vector>

namespace qt
{
    osc_freq_t::osc_freq_t( QWidget* parent ) : QGroupBox( parent )
    {
        auto layout = new QGridLayout;

        auto amplitude_poti = new poti_t;
        amplitude_poti->setTitle( "Amplitude" );
        amplitude_poti->dial().setMinimum( -150 );
        amplitude_poti->dial().setMaximum( 150 );
        amplitude_poti->dial().setNotchTarget( 10 );
        QObject::connect( amplitude_poti, &poti_t::value_changed, this,
                          &osc_freq_t::amplitude_changed );
        amplitude_poti->update( 100 );
        layout->addWidget( amplitude_poti, 0, 0 );

        auto phase_poti = new poti_t;
        phase_poti->setTitle( "Phase" );
        phase_poti->dial().setMinimum( 0 );
        phase_poti->dial().setMaximum( 99 );
        phase_poti->dial().setNotchTarget( 5 );
        QObject::connect( phase_poti, &poti_t::value_changed, this, &osc_freq_t::phase_changed );
        phase_poti->update( 0 );
        layout->addWidget( phase_poti, 0, 1 );

        auto wave_form_poti = new poti_t;
        std::vector< std::string > wave_forms = {"sine",  "tri",  "rect", "saw",
                                                 "pulse", "ramp", "noise"};
        wave_form_poti->setTitle( "Wave" );
        wave_form_poti->set_str_map( wave_forms );
        wave_form_poti->dial().setMinimum( 0 );
        wave_form_poti->dial().setMaximum( int( wave_forms.size() - 1 ) );
        wave_form_poti->dial().setNotchTarget( 1 );
        QObject::connect( wave_form_poti, &poti_t::value_changed, this,
                          &osc_freq_t::wave_form_changed );
        wave_form_poti->update( 0 );
        layout->addWidget( wave_form_poti, 0, 2 );

        auto note_poti = new poti_t;
        note_poti->setTitle( "Frequency" );
        note_poti->set_decimals( 1 );
        note_poti->dial().setMinimum( 1 );
        note_poti->dial().setMaximum( 220000 );
        note_poti->dial().setNotchTarget( 220000 / 22 );
        QObject::connect( note_poti, &poti_t::value_changed, this, &osc_freq_t::frequency_changed );
        note_poti->update( 1, false );
        layout->addWidget( note_poti, 1, 0 );

        auto harmonic_poti = new poti_t;
        harmonic_poti->setTitle( "Note offset" );
        harmonic_poti->dial().setMinimum( -60 );
        harmonic_poti->dial().setMaximum( 60 );
        harmonic_poti->dial().setNotchTarget( 6 );
        QObject::connect( harmonic_poti, &poti_t::value_changed, this,
                          &osc_freq_t::note_offset_changed );
        harmonic_poti->update( 0, false );
        layout->addWidget( harmonic_poti, 1, 1 );

        auto pulse_length_poti = new poti_t;
        pulse_length_poti->setTitle( "Pulse Length" );
        pulse_length_poti->dial().setMinimum( 0 );
        pulse_length_poti->dial().setMaximum( 100 );
        pulse_length_poti->dial().setNotchTarget( 10 );
        QObject::connect( pulse_length_poti, &poti_t::value_changed, this,
                          &osc_freq_t::pulse_length_changed );
        pulse_length_poti->update( 50 );
        layout->addWidget( pulse_length_poti, 1, 2 );

        auto dry_wet_poti = new poti_t;
        dry_wet_poti->setTitle( "Dry/Wet" );
        dry_wet_poti->dial().setMinimum( 0 );
        dry_wet_poti->dial().setMaximum( 100 );
        dry_wet_poti->dial().setNotchTarget( 10 );
        QObject::connect( dry_wet_poti, &poti_t::value_changed, this,
                          &osc_freq_t::dry_wet_ratio_changed );
        dry_wet_poti->update( 0 );
        layout->addWidget( dry_wet_poti, 2, 0 );

        setLayout( layout );
    }

    void osc_freq_t::set_oscillator( oscillator_t* oscillator )
    {
        oscillator_ = oscillator;
        amplitude_changed(
            dynamic_cast< poti_t* >(
                dynamic_cast< QGridLayout* >( layout() )->itemAtPosition( 0, 0 )->widget() )
                ->dial()
                .value() );
        wave_form_changed(
            dynamic_cast< poti_t* >(
                dynamic_cast< QGridLayout* >( layout() )->itemAtPosition( 0, 1 )->widget() )
                ->dial()
                .value() );
        frequency_changed(
            dynamic_cast< poti_t* >(
                dynamic_cast< QGridLayout* >( layout() )->itemAtPosition( 1, 0 )->widget() )
                ->dial()
                .value() );
        pulse_length_changed(
            dynamic_cast< poti_t* >(
                dynamic_cast< QGridLayout* >( layout() )->itemAtPosition( 1, 2 )->widget() )
                ->dial()
                .value() );
    }

    void osc_freq_t::amplitude_changed( int i )
    {
        if ( oscillator_ )
        {
            oscillator_->set_amplitude( i / 100.0 );
        }
    }

    void osc_freq_t::wave_form_changed( int i )
    {
        if ( oscillator_ )
        {
            oscillator_->set_wave_form(
                static_cast< sequencer::audio::oscillator_wave_form >( i ) );
        }
    }
    void osc_freq_t::phase_changed( int phase )
    {
        if ( oscillator_ )
        {
            oscillator_->set_phase( phase );
        }
    }

    void osc_freq_t::frequency_changed( int frequency )
    {
        if ( oscillator_ )
        {
            oscillator_->set_frequency( frequency / 10.0 );

            auto note_offset_poti = dynamic_cast< poti_t* >(
                dynamic_cast< QGridLayout* >( layout() )->itemAtPosition( 1, 1 )->widget() );
            note_offset_poti->update( 10 * compute_note_offset(), false );
        }
    }

    void osc_freq_t::note_offset_changed( int note_offset )
    {
        using sequencer::audio::half_note_t;
        using sequencer::audio::increase_by_half_notes;
        if ( oscillator_ )
        {
            const auto freq =
                increase_by_half_notes( carrier_frequency_, half_note_t{note_offset} );
            oscillator_->set_frequency( freq );
            auto freq_poti = dynamic_cast< poti_t* >(
                dynamic_cast< QGridLayout* >( layout() )->itemAtPosition( 1, 0 )->widget() );
            freq_poti->update( freq * 10, false );
        }
    }

    void osc_freq_t::pulse_length_changed( int pulse_length )
    {
        if ( oscillator_ )
        {
            oscillator_->set_pulse_length( pulse_length / 100.0 );
        }
    }

    void osc_freq_t::set_carrier_frequency( double carrier )
    {
        carrier_frequency_ = carrier;
    }

    void osc_freq_t::dry_wet_ratio_changed( int ratio )
    {
        if ( oscillator_ )
        {
            oscillator_->set_dry_wet_ratio( ratio / 100.0f );
        }
    }

    double osc_freq_t::compute_note_offset() noexcept
    {
        return 12 * std::log2( oscillator_->frequency() / carrier_frequency_ );
    }
} // namespace qt
