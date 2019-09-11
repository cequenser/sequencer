#pragma once

#include "backend.hpp"
#include "signal_blocker.hpp"

#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>

namespace qt
{
    class track_t : public QGroupBox
    {
        Q_OBJECT
    public:
        explicit track_t( QWidget* parent );

        void clear();

        int fixed_size() const noexcept;

        int size() const noexcept;

        template < class F >
        void update( F f )
        {
            for ( auto i = 0; i < fixed_size(); ++i )
            {
                signal_blocker_t signal_blocker{( *this )[ i ]};
                ( *this )[ i ].setChecked( f( i ) );
            }
        }

        void update_on_scale_change( int size );

        QCheckBox& operator[]( int i );

        const QCheckBox& operator[]( int i ) const;

        template < class T, class F >
        void connect( T* t, F f )
        {
            for ( auto i = 0; i < fixed_size(); ++i )
            {
                QObject::connect( &( *this )[ i ], &QCheckBox::clicked, t, [f, i] { f( i ); } );
            }
        }

        int page_count() const noexcept;

        void set_backend( backend& backend );

    public slots:
        void step_changed( int i );
        void page_changed();

    private:
        void display_page();
        void update_displayed_steps();

        backend* backend_{nullptr};
        QLabel* label_{new QLabel};
        int current_page_{0};
        int page_count_{1};
        int size_{16};
    };
} // namespace qt
