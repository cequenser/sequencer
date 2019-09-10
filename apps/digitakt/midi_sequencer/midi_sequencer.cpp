#include "midi_sequencer.hpp"

#include "ui_midi_sequencer.h"

#include <cassert>
#include <poti.hpp>

midi_sequencer::midi_sequencer( QWidget* parent )
    : QMainWindow( parent ), clock_{sequencer::rtmidi::make_clock()},
      clock_done_{start_clock_in_thread( clock_,
                                         [this]( auto message ) {
                                             midiout_.sendMessage(
                                                 static_cast< const unsigned char* >(
                                                     static_cast< const void* >( &message ) ),
                                                 1 );
                                         } )},
      ui( new Ui::midi_sequencer )
{
    ui->setupUi( this );

    scan_available_ports();

    clock_ui_sync = new qt::dial_line_edit_sync( ui->midi_clock_bpm_dial, ui->midi_clock_bpm_edit,
                                                 "", " bpm", 10, 400 );
    connect( clock_ui_sync, &qt::dial_line_edit_sync::value_changed, this,
             &midi_sequencer::set_clock_bpm );
    clock_ui_sync->update( 1200 );
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

void midi_sequencer::scan_available_ports()
{
    ui->midi_clock_port_box->clear();
    ui->midi_clock_port_box->addItem( "none" );
    for ( auto id = 0u; id < midiout_.getPortCount(); ++id )
    {
        ui->midi_clock_port_box->addItem( midiout_.getPortName( id ).c_str() );
    }
}
