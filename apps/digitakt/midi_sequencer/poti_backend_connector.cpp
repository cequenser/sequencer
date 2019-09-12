#include "poti_backend_connector.hpp"

#include "poti.hpp"
#include "signal_blocker.hpp"

#include <sequencer/backend/digitakt.hpp>

#include <QDial>
#include <QLineEdit>

using sequencer::backend::digitakt_mode;

namespace qt
{
    void note_control::init( qt::poti_t& poti )
    {
        qt::signal_blocker_t signal_blocker{poti.dial()};
        poti.dial().setMinimum( -24 );
        poti.dial().setMaximum( 24 );
        poti.dial().setNotchTarget( 49 / 16 );
        poti.setTitle( "Note" );
        poti.line_edit().setAlignment( Qt::AlignRight );
        poti.update( 0 );
    }

    void note_control::update( sequencer::backend::digitakt& backend, qt::poti_t& poti )
    {
        const auto& current_track = backend.current_track();
        if ( backend.mode() == digitakt_mode::step_select )
        {
            const auto& step_note = current_track[ backend.current_step() ].note();
            auto half_note_difference =
                step_note ? get_note_distance( current_track.base_note(), step_note->load() )
                          : current_track.note_offset();
            poti.update( half_note_difference );
            return;
        }

        poti.update( current_track.note_offset() );
        poti.setTitle( "Note" );
    }

    void velocity_control::init( poti_t& poti )
    {
        qt::signal_blocker_t signal_blocker{poti.dial()};
        poti.dial().setNotchTarget( 128 / 16 );
        poti.dial().setMinimum( 1 );
        poti.dial().setMaximum( 127 );
        poti.setTitle( "Velocity" );
        poti.line_edit().setAlignment( Qt::AlignRight );
        poti.update( 100 );
    }

    void velocity_control::update( sequencer::backend::digitakt& backend, qt::poti_t& poti )
    {
        const auto& current_track = backend.current_track();
        if ( backend.mode() == digitakt_mode::step_select )
        {
            const auto& step_velocity = current_track[ backend.current_step() ].velocity();
            const auto velocity = step_velocity ? step_velocity->load() : current_track.velocity();
            poti.update( velocity );
            return;
        }

        poti.update( current_track.velocity() );
        poti.setTitle( "Velocity" );
    }
} // namespace qt
