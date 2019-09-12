#include "poti_group.hpp"

#include "poti.hpp"

#include <QDial>
#include <QGridLayout>
#include <iostream>
namespace qt
{
    poti_group_t::poti_group_t( QWidget* parent ) : QGroupBox( parent )
    {
    }

    void poti_group_t::set_number_of_potis( unsigned potis_count, unsigned row_count )
    {
        auto layout = new QGridLayout;
        const auto row_size = potis_count / row_count + bool( potis_count % row_count );
        for ( auto i = 0u; i < potis_count; ++i )
        {
            auto poti = new poti_t{};
            poti->dial().setMaximum( 127 );
            poti->dial().setNotchTarget( 8 );
            poti->dial().setNotchesVisible( true );
            poti->set_threshold( 10 );
            layout->addWidget( poti, i / row_size, i % row_size );
        }

        setLayout( layout );
    }

    poti_t& poti_group_t::operator[]( int idx )
    {
        return static_cast< poti_t& >( *layout()->itemAt( idx )->widget() );
    }

    const poti_t& poti_group_t::operator[]( int idx ) const
    {
        return static_cast< const poti_t& >( *layout()->itemAt( idx )->widget() );
    }
} // namespace qt
