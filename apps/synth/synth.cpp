#include "synth.hpp"

#include "poti.hpp"
#include "ui_synth.h"

#include <sequencer/beat_duration.hpp>

#include <QDial>

synth::synth( QWidget* parent, void ( *callback )( double, std::vector< unsigned char >*, void* ) )
    : QMainWindow( parent ), ui( new Ui::synth ), backend_{callback}
{
    ui->setupUi( this );

    ui->osc_1_box->set_chain( &backend_.chain( 0 ) );
    ui->osc_2_box->set_chain( &backend_.chain( 1 ) );

    ui->audio_device_box->addItem( "select audio device" );
    for ( const auto& device_name : backend_.available_audio_devices() )
    {
        ui->audio_device_box->addItem( device_name.c_str() );
    }

    ui->midiin_box->addItem( "select midi input port" );
    for ( const auto& port_name : backend_.available_input_ports() )
    {
        ui->midiin_box->addItem( port_name.c_str() );
    }

    backend_.set_bpm_functor( [this]( sequencer::beat_duration bpm ) {
        ui->bpm_label->setText( QString::number( bpm.to_double() ) + " bpm" );
    } );

    ui->reverb_folder_box->addItem( "select reverb bank" );
    const auto reverb_types = backend_.available_reverb_banks();
    for ( auto& reverb_type : reverb_types )
    {
        ui->reverb_folder_box->addItem( reverb_type.c_str() );
    }

    ui->dry_wet_poti->dial().setMinimum( 0 );
    ui->dry_wet_poti->dial().setMaximum( 1000 );
    ui->dry_wet_poti->set_decimals( 1 );
    ui->dry_wet_poti->dial().setNotchTarget( 10 );
    ui->dry_wet_poti->update( 0 );
    connect( ui->dry_wet_poti, &qt::poti_t::value_changed, this, &synth::reverb_dry_wet_changed );
}

synth::~synth()
{
    delete ui;
}

synth::backend_t& synth::backend() noexcept
{
    return backend_;
}

void synth::play()
{
    play_ = !play_;
    play_ ? backend_.start() : backend_.stop();
}

void synth::trig()
{
    backend_.trigger();
}

void synth::audio_device_changed( int device_id )
{
    if ( device_id == 0 )
    {
        return;
    }
    backend_.set_device( device_id - 1 );
}

void synth::midi_port_changed( int port_id )
{
    backend_.select_input_port( port_id );
}

void synth::reverb_type_changed( int id )
{
    backend_.set_reverb_type_id( id );
}

void synth::reverb_bank_changed( int id )
{
    backend_.set_reverb_bank_id( id );
    ui->reverb_type_box->clear();
    ui->reverb_type_box->addItem( "select reverb" );
    const auto reverbs = backend_.available_reverbs();
    for ( auto& reverb : reverbs )
    {
        ui->reverb_type_box->addItem( reverb.c_str() );
    }
}

void synth::reverb_dry_wet_changed( int dry_wet_ratio )
{
    backend_.set_dry_wet_ratio( ( 1000 - dry_wet_ratio ) / 1000.0 );
}
