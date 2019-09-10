#include "poti.hpp"

#include <QDial>
#include <QLineEdit>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <signal_blocker.hpp>
#include <sstream>

namespace qt
{
    dial_line_edit_sync::dial_line_edit_sync( QDial* dial, QLineEdit* line_edit, QString prefix,
                                              QString suffix, int floating_factor, int threshold )
        : dial_{dial}, line_edit_{line_edit}, prefix_{prefix}, suffix_{suffix},
          floating_factor_{floating_factor}, threshold_{threshold}
    {
        connect( dial_, &QDial::valueChanged, this, &dial_line_edit_sync::update_from_dial );
        connect( line_edit_, &QLineEdit::editingFinished, this,
                 &dial_line_edit_sync::update_from_line_edit );
    }

    void dial_line_edit_sync::update( int value )
    {
        const auto bpm = double( value ) / floating_factor_;
        std::stringstream stream;
        stream << std::setprecision( 1 ) << std::fixed << bpm;
        line_edit().setText( prefix_ + QString( stream.str().c_str() ) + suffix_ );
        old_value_ = value;

        emit value_changed( bpm );
    }

    void dial_line_edit_sync::update_from_dial( int value )
    {
        if ( std::abs( value - old_value_ ) > threshold_ )
        {
            if ( old_value_ == dial().minimum() || old_value_ == dial().maximum() )
            {
                value = old_value_;
            }
            dial().setValue( value );
        }

        update( value );
    }

    void dial_line_edit_sync::update_from_line_edit()
    {
        auto value = line_edit().text();
        auto x = int( value.remove( prefix_ ).remove( suffix_ ).trimmed().toDouble() *
                      floating_factor_ );
        x = std::min( std::max( x, dial().minimum() ), dial().maximum() );
        signal_blocker_t signal_blocker{dial()};
        dial().setValue( x );
        update( x );
    }

    QDial& dial_line_edit_sync::dial()
    {
        return *dial_;
    }

    QLineEdit& dial_line_edit_sync::line_edit()
    {
        return *line_edit_;
    }

} // namespace qt
