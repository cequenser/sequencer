#include "midi_sequencer.hpp"

#include "poti.hpp"
#include "scale_dialog.hpp"
#include "signal_blocker.hpp"
#include "track.hpp"
#include "ui_midi_sequencer.h"
#include "util.hpp"

#include <QCheckBox>
#include <QDial>
#include <QHBoxLayout>
#include <QLineEdit>
#include <cassert>
#include <cmath>
#include <iostream>

using sequencer::backend::digitakt_control_mode;
using sequencer::backend::digitakt_mode;

namespace
{
    void init( qt::poti_t& poti, int min, int max, int value, const std::string& title,
               int decimals )
    {
        qt::signal_blocker_t signal_blocker{poti.dial()};
        poti.dial().setNotchTarget( ( 1 + max - min ) / 16.0 );
        poti.dial().setMinimum( min );
        poti.dial().setMaximum( max );
        poti.setTitle( title.c_str() );
        poti.line_edit().setAlignment( Qt::AlignRight );
        poti.set_decimals( decimals );
        poti.update( value );
    }
} // namespace

midi_sequencer::midi_sequencer( QWidget* parent )
    : QMainWindow( parent ), ui( new Ui::midi_sequencer )
{
    ui->setupUi( this );

    scan_available_ports();

    ui->sequencer_box->connect( this, [this]( auto i ) { sequencer_step_changed( i ); } );
    ui->sequencer_box->set_backend( backend_ );

    // init clock representation
    ui->clock_box->set_suffix( " bpm" );
    ui->clock_box->set_decimals( 1 );
    ui->clock_box->line_edit().setMinimumWidth( 80 );
    ui->clock_box->line_edit().setMaximumWidth( 80 );
    ui->clock_box->setTitle( "Clock" );
    connect( ui->clock_box, &qt::poti_t::value_changed, this, &midi_sequencer::set_clock_bpm );
    ui->clock_box->update( 1200 );

    const auto rows = 2;
    ui->control_box->set_number_of_potis( number_of_control_potis, rows );
    backend_.set_control_mode( digitakt_control_mode::trig );
    const auto& spec = backend_.spec();
    for ( auto index = 0u; index < number_of_control_potis; ++index )
    {
        init( ( *ui->control_box )[ index ], spec[ index ].min, spec[ index ].max,
              spec[ index ].value, spec[ index ].name, spec[ index ].decimals );
        connect( &( *ui->control_box )[ index ], &qt::poti_t::value_changed, this,
                 [this, index]( int value ) { control_poti_changed( int( index ), value ); } );
    }
}

midi_sequencer::~midi_sequencer()
{
    delete ui;
}

void midi_sequencer::start_clock()
{
    backend_.start_clock();
}

void midi_sequencer::stop_clock()
{
    backend_.stop_clock();
}

void midi_sequencer::set_clock_bpm( int value )
{
    backend_.set_clock_bpm( value );
}

void midi_sequencer::select_port( int idx )
{
    backend_.select_port( idx );
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
    const auto track_mode = backend_.mode() == digitakt_mode::bank_select
                                ? qt::track_mode::select
                                : qt::track_mode::sequencer;
    ui->sequencer_box->set_mode( track_mode );
    update_sequencer_steps();
}

void midi_sequencer::change_pattern()
{
    backend_.set_mode( digitakt_mode::pattern_select );
    update_buttons();
    const auto track_mode = backend_.mode() == digitakt_mode::pattern_select
                                ? qt::track_mode::select
                                : qt::track_mode::sequencer;
    ui->sequencer_box->set_mode( track_mode );
    update_sequencer_steps();
}

void midi_sequencer::change_track()
{
    backend_.set_mode( digitakt_mode::track_select );
    update_buttons();
    const auto track_mode = backend_.mode() == digitakt_mode::track_select
                                ? qt::track_mode::select
                                : qt::track_mode::sequencer;
    ui->sequencer_box->set_mode( track_mode );
    update_sequencer_steps();
}

void midi_sequencer::sequencer_step_changed( int idx )
{
    if ( backend_.mode() != digitakt_mode::play )
    {
        backend_.set_step( idx );
        ui->sequencer_box->set_mode( qt::track_mode::sequencer );
        update_buttons();
        update_sequencer_steps();
        update_potis();
        return;
    }

    ui->sequencer_box->step_changed( idx );
    update_potis();
}

void midi_sequencer::trig_selected()
{
    qt::use_secondary_function() ? backend_.set_control_mode( digitakt_control_mode::quantize )
                                 : backend_.set_control_mode( digitakt_control_mode::trig );
    update_potis();
}

void midi_sequencer::source_selected()
{
    qt::use_secondary_function() ? backend_.set_control_mode( digitakt_control_mode::assign )
                                 : backend_.set_control_mode( digitakt_control_mode::source );
    update_potis();
}

void midi_sequencer::filter_selected()
{
    qt::use_secondary_function() ? backend_.set_control_mode( digitakt_control_mode::delay )
                                 : backend_.set_control_mode( digitakt_control_mode::filter );
    update_potis();
}

void midi_sequencer::amp_selected()
{
    qt::use_secondary_function() ? backend_.set_control_mode( digitakt_control_mode::reverb )
                                 : backend_.set_control_mode( digitakt_control_mode::amp );
    update_potis();
}

void midi_sequencer::lfo_selected()
{
    qt::use_secondary_function() ? backend_.set_control_mode( digitakt_control_mode::master )
                                 : backend_.set_control_mode( digitakt_control_mode::lfo );
    update_potis();
}

void midi_sequencer::control_poti_changed( int id, int value )
{
    backend_.set_control( id, value );
}

void midi_sequencer::scan_available_ports()
{
    ui->midi_clock_port_box->clear();
    ui->midi_clock_port_box->addItem( "select midi port" );
    const auto ports = backend_.available_ports();
    for ( const auto& port : ports )
    {
        ui->midi_clock_port_box->addItem( port.c_str() );
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

void midi_sequencer::update_potis()
{
    const auto& spec = backend_.spec();
    for ( auto index = 0u; index < number_of_control_potis; ++index )
    {
        init( ( *ui->control_box )[ index ], spec[ index ].min, spec[ index ].max,
              backend_.get_control_value( index ), spec[ index ].name, spec[ index ].decimals );
    }
}
