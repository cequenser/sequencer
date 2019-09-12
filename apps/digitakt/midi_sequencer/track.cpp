#include "track.hpp"

#include "scale_dialog.hpp"
#include "util.hpp"

#include <QGridLayout>
#include <QPushButton>
#include <algorithm>
#include <cassert>

namespace qt
{
    track_t::track_t( QWidget* parent ) : QGroupBox( parent )
    {
        auto seq_layout = new QGridLayout;
        for ( auto i = 0; i < fixed_size(); ++i )
        {
            seq_layout->addWidget( new QCheckBox, i / ( fixed_size() / 2 ),
                                   i % ( fixed_size() / 2 ) );
        }
        auto button = new QPushButton;
        button->setText( "Page" );
        QObject::connect( button, &QPushButton::clicked, this, &track_t::page_changed );
        seq_layout->addWidget( button, 1, fixed_size() / 2 );
        label_->setAlignment( Qt::AlignRight );
        seq_layout->addWidget( label_, 0, fixed_size() / 2 );
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
        return backend_->current_track().steps();
    }

    void track_t::update_on_scale_change( int size )
    {
        assert( size > 0 );

        page_count_ = size / fixed_size() + bool( size % fixed_size() );
        current_page_ = page_count() - 1;
        update();
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

    void track_t::set_backend( sequencer::backend::digitakt& backend )
    {
        backend_ = &backend;
    }

    void track_t::update()
    {
        display_page();
        update_displayed_steps();
    }

    void track_t::set_mode( track_mode mode ) noexcept
    {
        mode_ = mode;
    }

    void track_t::step_changed( int idx )
    {
        set_mode( track_mode::sequencer );
        if ( qt::use_secondary_function() )
        {
            {
                qt::signal_blocker_t signal_blocker{( *this )[ idx ]};
                ( *this )[ idx ].setChecked( !( *this )[ idx ].isChecked() );
            }
            backend_->set_mode( sequencer::backend::digitakt_mode::step_select );
            if ( backend_->mode() == sequencer::backend::digitakt_mode::step_select )
            {
                enable( [idx]( auto i ) { return i == idx; } );
            }
        }

        backend_->set_step( idx + fixed_size() * current_page_, ( *this )[ idx ].isChecked() );
        update();
    }

    void track_t::page_changed()
    {
        if ( qt::use_secondary_function() && backend_ )
        {
            auto dialog =
                qt::scale_dialog_t{backend_->current_pattern(), backend_->current_track(), *this};
            dialog.exec();
            return;
        }

        if ( backend_->mode() == sequencer::backend::digitakt_mode::step_select )
        {
            backend_->set_mode( sequencer::backend::digitakt_mode::step_select );
        }
        if ( ++current_page_ == page_count() )
        {
            current_page_ = 0;
        }
        update();
    }

    void track_t::display_page()
    {
        label_->setText( QString::number( 1 + current_page_ ) + "/" +
                         QString::number( page_count() ) );
    }

    void track_t::update_displayed_steps()
    {
        if ( mode_ == track_mode::select )
        {
            clear();
            return;
        }

        if ( backend_->mode() != sequencer::backend::digitakt_mode::step_select )
        {
            enable( []( auto ) { return true; } );
        }

        if ( backend_->mode() == sequencer::backend::digitakt_mode::mute )
        {
            update( [this]( auto i ) { return backend_->current_pattern()[ i ].is_muted(); } );
        }
        else
        {
            update( [this]( auto i ) {
                const auto idx = std::size_t( i + fixed_size() * current_page_ );
                if ( idx < backend_->current_track().steps() )
                {
                    return backend_->current_track()[ idx ].is_active();
                }

                return false;
            } );
        }

        const auto offset = current_page_ * fixed_size();
        const auto end = std::min( fixed_size(), int( size() - offset ) );

        for ( auto i = end; i < fixed_size(); ++i )
        {
            ( *this )[ i ].setEnabled( false );
        }
    }
} // namespace qt
