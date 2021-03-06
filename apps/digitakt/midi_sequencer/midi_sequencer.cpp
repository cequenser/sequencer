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

namespace digitakt = sequencer::backend::digitakt;
using digitakt::control_mode_t;
using digitakt::mode_t;

namespace
{
    void init( qt::poti_t& poti, int min, int max, int value, const std::string& title,
               int decimals, const std::vector< std::string >& str_map,
               const std::vector< int >& map )
    {
        qt::signal_blocker_t signal_blocker{poti.dial()};
        qt::signal_blocker_t line_signal_blocker{poti.line_edit()};
        poti.dial().setNotchTarget( ( 1 + max - min ) / 16.0 );
        poti.dial().setMinimum( min );
        poti.dial().setMaximum( max );
        poti.setTitle( title.c_str() );
        poti.line_edit().setAlignment( Qt::AlignRight );
        poti.set_decimals( decimals );
        poti.set_str_map( str_map );
        poti.set_map( map );
        poti.update( value, false );
    }

    void init( qt::poti_t& poti, int value, const sequencer::midi::device_entry_t& entry )
    {
        init( poti, entry.min, entry.max, value, entry.name, entry.decimals, entry.str_map,
              entry.map );
    }
} // namespace

midi_sequencer::midi_sequencer( QWidget* parent,
                                void ( *callback )( double, std::vector< unsigned char >*, void* ) )
    : QMainWindow( parent ), backend_{callback}, ui( new Ui::midi_sequencer )
{
    ui->setupUi( this );

    update_clock_ports();

    ui->midi_output_port_box->clear();
    ui->midi_output_port_box->addItem( "select output port" );
    for ( const auto& port : backend_.available_output_ports() )
    {
        ui->midi_output_port_box->addItem( port.c_str() );
    }

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
    backend_.set_control_mode( control_mode_t::trig );
    const auto& spec = backend_.spec();
    for ( auto index = 0u;
          index < std::min( spec.size(), decltype( spec.size() )( number_of_control_potis ) );
          ++index )
    {
        init( ( *ui->control_box )[ index ], spec[ index ].value, spec[ index ] );
    }
    for ( auto index = 0u; index < number_of_control_potis; ++index )
    {
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
    backend_.set_clock_bpm( value / 10.0 );
}

void midi_sequencer::select_clock_port( int idx )
{
    if ( is_clock_slave_ )
    {
        backend_.clock_receiver.select_input_port( idx );
        return;
    }

    backend_.clock_receiver.select_input_port( idx );
    backend_.clock_sender.select_output_port( idx );
}

void midi_sequencer::select_output_port( int idx )
{
    backend_.select_output_port( idx );
}

void midi_sequencer::set_slave_mode( bool slave )
{
    is_clock_slave_ = slave;
    update_clock_ports();
}

void midi_sequencer::change_bank()
{
    if ( qt::use_secondary_function() )
    {
        backend_.set_mode( digitakt::mode_t::mute );
        update_buttons();
        update_sequencer_steps();
        return;
    }

    backend_.set_mode( digitakt::mode_t::bank_select );
    update_buttons();
    const auto track_mode = backend_.mode() == digitakt::mode_t::bank_select
                                ? qt::track_mode::select
                                : qt::track_mode::sequencer;
    ui->sequencer_box->set_mode( track_mode );
    update_sequencer_steps();
}

void midi_sequencer::change_pattern()
{
    backend_.set_mode( digitakt::mode_t::pattern_select );
    update_buttons();
    const auto track_mode = backend_.mode() == digitakt::mode_t::pattern_select
                                ? qt::track_mode::select
                                : qt::track_mode::sequencer;
    ui->sequencer_box->set_mode( track_mode );
    update_sequencer_steps();
}

void midi_sequencer::change_track()
{
    backend_.set_mode( digitakt::mode_t::track_select );
    update_buttons();
    const auto track_mode = backend_.mode() == digitakt::mode_t::track_select
                                ? qt::track_mode::select
                                : qt::track_mode::sequencer;
    ui->sequencer_box->set_mode( track_mode );
    update_sequencer_steps();
}

void midi_sequencer::sequencer_step_changed( int idx )
{
    if ( backend_.mode() != digitakt::mode_t::play )
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

#define SEQUENCER_SELECT_CONTROL_MODE( primary, secondary )                                        \
    if ( qt::use_secondary_function() )                                                            \
    {                                                                                              \
        backend_.set_control_mode( control_mode_t::secondary );                                    \
        ui->control_box->setTitle( #secondary );                                                   \
    }                                                                                              \
    else                                                                                           \
    {                                                                                              \
        backend_.set_control_mode( control_mode_t::primary );                                      \
        ui->control_box->setTitle( #primary );                                                     \
    }                                                                                              \
    update_potis();

void midi_sequencer::trig_selected()
{
    SEQUENCER_SELECT_CONTROL_MODE( trig, quantize )
}

void midi_sequencer::source_selected()
{
    SEQUENCER_SELECT_CONTROL_MODE( source, assign )
}

void midi_sequencer::filter_selected()
{
    SEQUENCER_SELECT_CONTROL_MODE( filter, delay )
}

void midi_sequencer::amp_selected()
{
    SEQUENCER_SELECT_CONTROL_MODE( amp, reverb )
}

void midi_sequencer::lfo_selected()
{
    SEQUENCER_SELECT_CONTROL_MODE( lfo, master )
}
#undef SEQUENCER_SELECT_CONTROL_MODE

void midi_sequencer::control_poti_changed( int id, int value )
{
    backend_.set_control( id, value );
}

void midi_sequencer::update_clock_ports()
{
    qt::signal_blocker_t clock_box_blocker{*ui->midi_clock_port_box};
    qt::signal_blocker_t output_box_block{*ui->midi_output_port_box};
    ui->midi_clock_port_box->clear();
    if ( is_clock_slave_ )
    {
        ui->midi_clock_port_box->addItem( "select clock input port" );
        for ( const auto& port : backend_.clock_receiver.available_input_ports() )
        {
            ui->midi_clock_port_box->addItem( port.c_str() );
        }
    }
    else
    {
        ui->midi_clock_port_box->addItem( "select clock output port" );
        for ( const auto& port : backend_.clock_sender.available_output_ports() )
        {
            ui->midi_clock_port_box->addItem( port.c_str() );
        }
    }
}

void midi_sequencer::update_sequencer_steps()
{
    ui->sequencer_box->update();
}

void midi_sequencer::update_buttons()
{
    ui->bank_button->setEnabled( backend_.mode() != digitakt::mode_t::bank_select );
    ui->pattern_button->setEnabled( backend_.mode() != digitakt::mode_t::pattern_select );
    ui->track_button->setEnabled( backend_.mode() != digitakt::mode_t::track_select );

    ui->bank_button->setText( backend_.mode() == digitakt::mode_t::mute ? "Mute" : "Bank" );
}

void midi_sequencer::update_potis()
{
    const auto& spec = backend_.spec();
    for ( auto index = 0u; index < number_of_control_potis; ++index )
    {
        ( *ui->control_box )[ index ].setEnabled( true );
        if ( index < spec.size() )
        {
            init( ( *ui->control_box )[ index ], backend_.get_control_value( index ),
                  spec[ index ] );
        }
        else
        {
            ( *ui->control_box )[ index ].setEnabled( false );
        }
    }
}
