#include "midi_sequencer.hpp"

#include "signal_blocker.hpp"
#include "ui_midi_sequencer.h"

#include <QCheckBox>
#include <QHBoxLayout>
#include <cassert>
#include <dial_line_edit_sync.hpp>

namespace
{
    class sequencer_ui_control_t
    {
    public:
        explicit sequencer_ui_control_t( QGroupBox& sequencer_box ) noexcept
            : sequencer_box_{sequencer_box}
        {
        }

        void clear()
        {
            update( []( int ) { return false; } );
        }

        template < class F >
        void update( F f )
        {
            for ( auto i = 0; i < sequencer_box_.layout()->count(); ++i )
            {
                qt::signal_blocker_t signal_blocker{( *this )[ i ]};
                ( *this )[ i ].setChecked( f( i ) );
            }
        }

        QCheckBox& operator[]( int i )
        {
            return *static_cast< QCheckBox* >( sequencer_box_.layout()->itemAt( i )->widget() );
        }

    private:
        QGroupBox& sequencer_box_;
    };

    sequencer_ui_control_t* sequencer_ui_control;
} // namespace

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

    auto seq_layout = new QHBoxLayout;
    for ( auto i = 0; i < 16; ++i )
    {
        auto step = new QCheckBox;
        connect( step, &QCheckBox::clicked, this,
                 [this, i] { this->sequencer_step_changed( i ); } );
        seq_layout->addWidget( step );
    }
    ui->sequencer_box->setLayout( seq_layout );
    sequencer_ui_control = new sequencer_ui_control_t{*ui->sequencer_box};

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

void midi_sequencer::change_bank()
{
    enable_all_buttons();
    if ( use_secondary_function() )
    {
        if ( backend_.mode() == backend_mode::mute )
        {
            reset_mode();
        }
        else
        {
            backend_.set_mode( backend_mode::mute );
            ui->bank_button->setText( "Mute" );
        }
        update_sequencer_steps();
        return;
    }

    reset_mode();
    ui->bank_button->setEnabled( false );
    sequencer_ui_control->clear();
}

void midi_sequencer::change_pattern()
{
    reset_mode();
    enable_all_buttons();
    ui->pattern_button->setEnabled( false );
    sequencer_ui_control->clear();
}

void midi_sequencer::change_track()
{
    reset_mode();
    enable_all_buttons();
    ui->track_button->setEnabled( false );
    sequencer_ui_control->clear();
}

void midi_sequencer::sequencer_step_changed( int idx )
{
    if ( backend_.mode() == backend_mode::play )
    {
        if ( !ui->bank_button->isEnabled() )
        {
            backend_.set_current_bank( idx );
            ui->bank_button->setEnabled( true );
            change_pattern();
            return;
        }

        if ( !ui->pattern_button->isEnabled() )
        {
            backend_.set_current_pattern( idx );
            ui->pattern_button->setEnabled( true );
            update_sequencer_steps();
            return;
        }

        if ( !ui->track_button->isEnabled() )
        {
            backend_.set_current_track( idx );
            ui->track_button->setEnabled( true );
            update_sequencer_steps();
            return;
        }
    }

    backend_.set_step( idx, ( *sequencer_ui_control )[ idx ].isChecked() );
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
    sequencer_ui_control->update( [this]( auto step ) { return backend_.is_step_set( step ); } );
}

void midi_sequencer::enable_all_buttons()
{
    ui->bank_button->setEnabled( true );
    ui->pattern_button->setEnabled( true );
    ui->track_button->setEnabled( true );
}

bool midi_sequencer::use_secondary_function()
{
    return QApplication::keyboardModifiers().testFlag( Qt::ControlModifier );
}

void midi_sequencer::reset_mode()
{
    backend_.set_mode( backend_mode::play );
    ui->bank_button->setText( "Bank" );
}
