#include "osc.hpp"

#include "poti.hpp"

#include <sequencer/audio/note.hpp>

#include <QDial>
#include <QGridLayout>
#include <string>
#include <vector>

namespace qt
{
    osc_t::osc_t( QWidget* parent ) : QGroupBox( parent )
    {
        auto layout = new QGridLayout;

        auto amplitude_poti = new poti_t;
        amplitude_poti->setTitle( "Amplitude" );
        amplitude_poti->dial().setMinimum( 0 );
        amplitude_poti->dial().setMaximum( 150 );
        amplitude_poti->dial().setNotchTarget( 10 );
        QObject::connect( amplitude_poti, &poti_t::value_changed, this, &osc_t::amplitude_changed );
        amplitude_poti->update( 100 );
        layout->addWidget( amplitude_poti, 0, 0 );

        auto phase_poti = new poti_t;
        phase_poti->setTitle( "Phase" );
        phase_poti->dial().setMinimum( 0 );
        phase_poti->dial().setMaximum( 99 );
        phase_poti->dial().setNotchTarget( 5 );
        QObject::connect( phase_poti, &poti_t::value_changed, this, &osc_t::phase_changed );
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
        QObject::connect( wave_form_poti, &poti_t::value_changed, this, &osc_t::wave_form_changed );
        wave_form_poti->update( 0 );
        layout->addWidget( wave_form_poti, 0, 2 );

        auto note_poti = new poti_t;
        std::vector< std::string > notes = {"A",  "A#", "B", "C",  "C#", "D",
                                            "D#", "E",  "F", "F#", "G",  "G#"};
        note_poti->setTitle( "Note" );
        note_poti->set_str_map( notes );
        note_poti->dial().setMinimum( 0 );
        note_poti->dial().setMaximum( int( notes.size() - 1 ) );
        note_poti->dial().setNotchTarget( 1 );
        QObject::connect( note_poti, &poti_t::value_changed, this, &osc_t::note_changed );
        note_poti->update( 0 );
        layout->addWidget( note_poti, 1, 0 );

        auto octave_poti = new poti_t;
        octave_poti->setTitle( "Octave" );
        octave_poti->dial().setMinimum( -16 );
        octave_poti->dial().setMaximum( 8 );
        octave_poti->dial().setNotchTarget( 1 );
        QObject::connect( octave_poti, &poti_t::value_changed, this, &osc_t::octave_changed );
        octave_poti->update( 0 );
        layout->addWidget( octave_poti, 1, 1 );

        auto pulse_length_poti = new poti_t;
        pulse_length_poti->setTitle( "Pulse Length" );
        pulse_length_poti->dial().setMinimum( 0 );
        pulse_length_poti->dial().setMaximum( 100 );
        pulse_length_poti->dial().setNotchTarget( 10 );
        QObject::connect( pulse_length_poti, &poti_t::value_changed, this,
                          &osc_t::pulse_length_changed );
        pulse_length_poti->update( 50 );
        layout->addWidget( pulse_length_poti, 1, 2 );

        setLayout( layout );
    }

    void osc_t::set_oscillator( sequencer::audio::oscillator_t* oscillator )
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
        note_changed(
            dynamic_cast< poti_t* >(
                dynamic_cast< QGridLayout* >( layout() )->itemAtPosition( 1, 0 )->widget() )
                ->dial()
                .value() );
        octave_changed(
            dynamic_cast< poti_t* >(
                dynamic_cast< QGridLayout* >( layout() )->itemAtPosition( 1, 1 )->widget() )
                ->dial()
                .value() );
        pulse_length_changed(
            dynamic_cast< poti_t* >(
                dynamic_cast< QGridLayout* >( layout() )->itemAtPosition( 1, 2 )->widget() )
                ->dial()
                .value() );
    }

    double osc_t::frequency() const noexcept
    {
        if ( oscillator_ )
        {
            return oscillator_->frequency();
        }
        return 0.0;
    }

    void osc_t::amplitude_changed( int i )
    {
        if ( oscillator_ )
        {
            oscillator_->set_amplitude( i / 100.0 );
        }
    }

    void osc_t::wave_form_changed( int i )
    {
        if ( oscillator_ )
        {
            oscillator_->set_wave_form(
                static_cast< sequencer::audio::oscillator_wave_form >( i ) );
        }
    }
    void osc_t::phase_changed( int phase )
    {
        if ( oscillator_ )
        {
            oscillator_->set_phase( phase );
        }
    }

    void osc_t::note_changed( int note )
    {
        note_idx_ = note;
        update_frequency();
    }

    void osc_t::octave_changed( int octave )
    {
        octave_offset_ = octave;
        update_frequency();
    }

    void osc_t::pulse_length_changed( int pulse_length )
    {
        if ( oscillator_ )
        {
            oscillator_->set_pulse_length( pulse_length / 100.0 );
        }
    }

    void osc_t::update_frequency()
    {
        using namespace sequencer::audio;
        using namespace sequencer::audio::base_notes;
        static std::array< double, 12 > base_notes = {A, Ais, B, C, Cis, D, Dis, E, F, Fis, G, Gis};
        if ( oscillator_ )
        {
            const auto freq =
                note_t{base_notes[ std::size_t( note_idx_ ) ], octave_offset_}.frequency();
            oscillator_->set_frequency( freq );

            emit frequency_changed( freq );
        }
    }
} // namespace qt
