#include "midi_sequencer.hpp"

#include "poti.hpp"
#include "scale_dialog.hpp"
#include "signal_blocker.hpp"
#include "track.hpp"
#include "ui_midi_sequencer.h"
#include "util.hpp"

#include <QCheckBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <cassert>

using sequencer::backend::digitakt_mode;

midi_sequencer::midi_sequencer( QWidget* parent )
    : QMainWindow( parent ), clock_{sequencer::rtmidi::make_clock()},
      clock_done_{start_clock_in_thread(
          clock_,
          [this]( auto message ) {
              midiout_.sendMessage(
                  static_cast< const unsigned char* >( static_cast< const void* >( &message ) ),
                  1 );
              backend_.receive_clock_message( message,
                                              sequencer::rtmidi::message_sender{midiout_} );
          } )},
      ui( new Ui::midi_sequencer )
{
    ui->setupUi( this );

    scan_available_ports();

    ui->sequencer_box->connect( this, [this]( auto i ) { sequencer_step_changed( i ); } );
    ui->sequencer_box->set_backend( backend_ );

    ui->control_box->set_number_of_potis( 8, 2 );

    // init clock representation
    ui->clock_box->set_suffix( " bpm" );
    ui->clock_box->set_floating_factor( 10 );
    ui->clock_box->set_threshold( 400 );
    ui->clock_box->line_edit().setMinimumWidth( 80 );
    ui->clock_box->line_edit().setMaximumWidth( 80 );
    ui->clock_box->setTitle( "Clock" );
    connect( ui->clock_box, &qt::poti_t::value_changed, this, &midi_sequencer::set_clock_bpm );
    ui->clock_box->update( 1200 );
}

midi_sequencer::~midi_sequencer()
{
    clock_.shut_down();
    delete ui;
}

void midi_sequencer::start_clock()
{
    clock_.start();
}

void midi_sequencer::stop_clock()
{
    if ( !clock_.is_running() )
    {
        clock_.reset();
        return;
    }

    clock_.stop();
}

void midi_sequencer::set_clock_bpm( double value )
{
    clock_.set_tempo( sequencer::beats_per_minute( value ) );
}

void midi_sequencer::select_port( int idx )
{
    assert( idx >= 0 );
    assert( idx <= int( midiout_.getPortCount() ) + 1 );

    if ( midiout_.isPortOpen() )
    {
        midiout_.closePort();
    }
    if ( idx == 0 )
    {
        return;
    }
    midiout_.openPort( unsigned( idx - 1 ) );
}

void midi_sequencer::change_bank()
{
    if ( qt::use_secondary_function() )
    {
        backend_.set_mode( digitakt_mode::mute );
        update_buttons();
        update_sequencer_steps();
        return;
    }

    backend_.set_mode( digitakt_mode::bank_select );
    update_buttons();
    ui->sequencer_box->clear();
}

void midi_sequencer::change_pattern()
{
    backend_.set_mode( digitakt_mode::pattern_select );
    update_buttons();
    ui->sequencer_box->clear();
}

void midi_sequencer::change_track()
{
    backend_.set_mode( digitakt_mode::track_select );
    update_buttons();
    ui->sequencer_box->clear();
}

void midi_sequencer::sequencer_step_changed( int idx )
{
    if ( backend_.mode() != digitakt_mode::play )
    {
        backend_.set_step( idx );
        update_buttons();
        update_sequencer_steps();
        return;
    }

    ui->sequencer_box->step_changed( idx );
    update_sequencer_steps();
}

void midi_sequencer::scan_available_ports()
{
    ui->midi_clock_port_box->clear();
    ui->midi_clock_port_box->addItem( "select midi port" );
    for ( auto id = 0u; id < midiout_.getPortCount(); ++id )
    {
        ui->midi_clock_port_box->addItem( midiout_.getPortName( id ).c_str() );
    }
}

void midi_sequencer::update_sequencer_steps()
{
    ui->sequencer_box->update();
}

void midi_sequencer::update_buttons()
{
    ui->bank_button->setEnabled( backend_.mode() != digitakt_mode::bank_select );
    ui->pattern_button->setEnabled( backend_.mode() != digitakt_mode::pattern_select );
    ui->track_button->setEnabled( backend_.mode() != digitakt_mode::track_select );

    ui->bank_button->setText( backend_.mode() == digitakt_mode::mute ? "Mute" : "Bank" );
}
