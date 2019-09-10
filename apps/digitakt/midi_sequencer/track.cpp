#include "track.hpp"

namespace qt
{
    track_t::track_t( QWidget* parent ) : QGroupBox( parent )
    {
        auto seq_layout = new QHBoxLayout;
        for ( auto i = 0; i < 16; ++i )
        {
            seq_layout->addWidget( new QCheckBox );
        }
        setLayout( seq_layout );
    }

    void track_t::clear()
    {
        update( []( int ) { return false; } );
    }

    QCheckBox& track_t::operator[]( int i )
    {
        return *static_cast< QCheckBox* >( layout()->itemAt( i )->widget() );
    }

    const QCheckBox& track_t::operator[]( int i ) const
    {
        return *static_cast< const QCheckBox* >( layout()->itemAt( i )->widget() );
    }
} // namespace qt
