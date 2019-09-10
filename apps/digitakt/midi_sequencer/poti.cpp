#include "poti.hpp"

#include "signal_blocker.hpp"

#include <QDial>
#include <QLineEdit>
#include <QString>
#include <QVBoxLayout>
#include <iomanip>
#include <sstream>

namespace qt
{
    poti_t::poti_t( QGroupBox* box ) : QGroupBox( box )
    {
        auto layout = new QVBoxLayout;
        dial_ = new QDial;
        dial().setMinimum( 0 );
        dial().setMaximum( 4800 );
        dial().setNotchTarget( 40 );
        dial().setNotchesVisible( true );
        const auto max_width = 100;
        dial().setMaximumWidth( max_width );
        layout->addWidget( dial_ );
        line_edit_ = new QLineEdit;
        line_edit().setMaximumWidth( max_width );
        layout->addWidget( line_edit_ );
        setLayout( layout );

        connect( dial_, &QDial::valueChanged, this, &poti_t::update_from_dial );
        connect( line_edit_, &QLineEdit::editingFinished, this, &poti_t::update_from_line_edit );
    }

    void poti_t::update( int value )
    {
        const auto bpm = double( value ) / floating_factor_;
        std::stringstream stream;
        stream << std::setprecision( 1 ) << std::fixed << bpm;
        line_edit().setText( QString( stream.str().c_str() ) + suffix_ );
        old_value_ = value;

        emit value_changed( bpm );
    }

    void poti_t::set_suffix( const QString& suffix )
    {
        suffix_ = suffix;
    }

    void poti_t::set_floating_factor( int factor )
    {
        floating_factor_ = factor;
    }

    void poti_t::set_threshold( int threshold )
    {
        threshold_ = threshold;
    }

    QDial& poti_t::dial()
    {
        return *dial_;
    }

    QLineEdit& poti_t::line_edit()
    {
        return *line_edit_;
    }

    void poti_t::update_from_dial( int value )
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

    void poti_t::update_from_line_edit()
    {
        auto value = line_edit().text();
        bool is_double = true;
        auto x = int( value.remove( suffix_ ).trimmed().toDouble( &is_double ) * floating_factor_ );
        if ( !is_double )
        {
            x = old_value_;
        }
        x = std::min( std::max( x, dial().minimum() ), dial().maximum() );
        signal_blocker_t signal_blocker{*dial_};
        dial().setValue( x );
        update( x );
    }
} // namespace qt
