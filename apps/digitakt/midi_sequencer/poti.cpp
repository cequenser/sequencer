#include "poti.hpp"

#include "signal_blocker.hpp"

#include <QDial>
#include <QLineEdit>
#include <QString>
#include <QVBoxLayout>
#include <cmath>
#include <iomanip>
#include <sstream>

namespace qt
{
    poti_t::poti_t( QWidget* parent ) : QGroupBox( parent )
    {
        auto layout = new QVBoxLayout;
        dial_ = new QDial;
        dial().setMinimum( 0 );
        dial().setMaximum( 4800 );
        dial().setNotchTarget( 40 );
        dial().setNotchesVisible( true );
        const auto min_width = 100;
        const auto max_width = 200;
        dial().setMinimumWidth( min_width );
        dial().setMaximumWidth( min_width );
        layout->addWidget( dial_ );
        line_edit_ = new QLineEdit;
        line_edit().setMinimumWidth( min_width );
        line_edit().setMaximumWidth( max_width );
        layout->addWidget( line_edit_ );
        setLayout( layout );

        connect( dial_, &QDial::valueChanged, this, &poti_t::update_from_dial );
        connect( line_edit_, &QLineEdit::editingFinished, this, &poti_t::update_from_line_edit );
    }

    void poti_t::update( int value, bool send_signals )
    {
        {
            signal_blocker_t signal_blocker( dial() );
            dial().setValue( value );
        }

        auto text_value = double( value );
        if ( !map_.empty() )
        {
            text_value = map_[ value ];
        }
        auto real_value = text_value / std::pow( 10, decimals_ );
        std::stringstream stream;
        stream << std::setprecision( decimals_ ) << std::fixed << real_value;
        if ( str_map_.empty() )
        {
            line_edit().setText( QString( stream.str().c_str() ) + suffix_ );
        }
        else
        {
            line_edit().setText(
                QString( str_map_[ std::size_t( value - dial().minimum() ) ].c_str() ) );
        }

        old_value_ = value;

        if ( !send_signals )
        {
            return;
        }

        emit value_changed( value );
    }

    void poti_t::set_suffix( const QString& suffix )
    {
        suffix_ = suffix;
    }

    void poti_t::set_decimals( int decimals )
    {
        decimals_ = decimals;
    }
    QDial& poti_t::dial()
    {
        return *dial_;
    }

    QLineEdit& poti_t::line_edit()
    {
        return *line_edit_;
    }

    void poti_t::set_str_map( const std::vector< std::string >& str_map )
    {
        str_map_ = str_map;
    }

    void poti_t::set_map( const std::vector< int >& map )
    {
        map_ = map;
    }

    void poti_t::update_from_dial( int value )
    {
        const auto threshold = std::max( ( dial().maximum() - dial().minimum() ) / 6, 6 );
        if ( std::abs( value - old_value_ ) > threshold )
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
        int x{0};
        if ( str_map_.empty() )
        {
            x = int( value.remove( suffix_ ).trimmed().toDouble( &is_double ) *
                     std::pow( 10, decimals_ ) );
            if ( !map_.empty() )
            {
                x = int( std::distance( begin( map_ ),
                                        std::find( begin( map_ ), end( map_ ), value ) ) );
            }
        }
        else
        {
            x = int(
                    std::distance( begin( str_map_ ), std::find( begin( str_map_ ), end( str_map_ ),
                                                                 value.toStdString() ) ) ) +
                dial().minimum();
        }
        if ( !is_double )
        {
            x = old_value_;
        }
        update( x );
    }
} // namespace qt
