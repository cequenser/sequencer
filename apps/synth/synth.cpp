#include "synth.hpp"

#include "poti.hpp"
#include "ui_synth.h"

#include <QDial>

synth::synth( QWidget* parent ) : QMainWindow( parent ), ui( new Ui::synth )
{
    ui->setupUi( this );

    ui->osc_1_box->set_chain( &backend_.chain( 0 ) );
    ui->osc_2_box->set_chain( &backend_.chain( 1 ) );

    ui->audio_device_box->addItem( "select audio device" );
    for ( const auto& device_name : backend_.available_audio_devices() )
    {
        ui->audio_device_box->addItem( device_name.c_str() );
    }
}

synth::~synth()
{
    delete ui;
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
