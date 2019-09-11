#include "track.hpp"

#include "scale_dialog.hpp"
#include "util.hpp"

#include <QPushButton>
#include <algorithm>
#include <cassert>

namespace qt
{
    track_t::track_t( QWidget* parent ) : QGroupBox( parent )
    {
        auto seq_layout = new QHBoxLayout;
        for ( auto i = 0u; i < fixed_size(); ++i )
        {
            seq_layout->addWidget( new QCheckBox );
        }
        auto button = new QPushButton;
        button->setText( "Page" );
        QObject::connect( button, &QPushButton::clicked, this, &track_t::page_changed );
        seq_layout->addWidget( button );
        label_->setAlignment( Qt::AlignRight );
        seq_layout->addWidget( label_ );
        setLayout( seq_layout );
        display_page();
    }

    void track_t::clear()
    {
        update( []( int ) { return false; } );
    }

    int track_t::fixed_size() const noexcept
    {
        return 16;
    }

    int track_t::size() const noexcept
    {
        return size_;
    }

    void track_t::update_on_scale_change( int size )
    {
        assert( size > 0 );
        size_ = size;

        for ( auto i = 0; i < fixed_size(); ++i )
        {
            ( *this )[ i ].setEnabled( true );
        }

        page_count_ = size / fixed_size() + bool( size % fixed_size() );
        current_page_ = page_count() - 1;
        display_page();
        update_displayed_steps();

        const auto offset = current_page_ * fixed_size();
        const auto end = std::min( layout()->count(), int( size - offset ) );

        for ( auto i = end; i < fixed_size(); ++i )
        {
            ( *this )[ i ].setEnabled( false );
        }
    }

    QCheckBox& track_t::operator[]( int i )
    {
        return *static_cast< QCheckBox* >( layout()->itemAt( i )->widget() );
    }

    const QCheckBox& track_t::operator[]( int i ) const
    {
        return *static_cast< const QCheckBox* >( layout()->itemAt( i )->widget() );
    }

    int track_t::page_count() const noexcept
    {
        return page_count_;
    }

    void track_t::set_backend( backend& backend )
    {
        backend_ = &backend;
    }

    void track_t::step_changed( int i )
    {
        backend_->set_step( i + fixed_size() * current_page_, ( *this )[ i ].isChecked() );
    }

    void track_t::page_changed()
    {
        if ( qt::use_secondary_function() && backend_ )
        {
            auto dialog = qt::scale_dialog_t{backend_->current_pattern(), *this};
            dialog.exec();
            return;
        }

        if ( ++current_page_ == page_count() )
        {
            current_page_ = 0;
        }
        display_page();
        update_displayed_steps();
    }

    void track_t::display_page()
    {
        label_->setText( QString::number( 1 + current_page_ ) + "/" +
                         QString::number( page_count() ) );
    }

    void track_t::update_displayed_steps()
    {
        update( [this]( auto i ) {
            const auto idx = i + fixed_size() * current_page_;
            if ( idx < backend_->current_track().steps() )
            {
                return backend_->current_track()[ idx ] != sequencer::midi::no_note();
            }

            return false;
        } );
    }
} // namespace qt
