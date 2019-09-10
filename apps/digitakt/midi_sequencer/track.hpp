#pragma once

#include "signal_blocker.hpp"

#include <QCheckBox>
#include <QGroupBox>
#include <QLayout>

namespace qt
{
    class track_t : public QGroupBox
    {
        Q_OBJECT
    public:
        explicit track_t( QWidget* parent );

        void clear();

        template < class F >
        void update( F f )
        {
            forEach( [this, f]( auto i ) {
                signal_blocker_t signal_blocker{( *this )[ i ]};
                ( *this )[ i ].setChecked( f( i ) );
            } );
        }

        QCheckBox& operator[]( int i );

        const QCheckBox& operator[]( int i ) const;

        template < class T >
        void connect( T* t )
        {
            forEach( [this, t]( auto i ) {
                QObject::connect( &( *this )[ i ], &QCheckBox::clicked, t,
                                  [t, i] { t->sequencer_step_changed( i ); } );
            } );
        }

    private:
        template < class F >
        void forEach( F f )
        {
            for ( auto i = 0; i < layout()->count(); ++i )
            {
                f( i );
            }
        }
    };
} // namespace qt
