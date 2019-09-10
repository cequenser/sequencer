#pragma once

#include <QGroupBox>

namespace qt
{
    class poti_t;

    class poti_group_t : public QGroupBox
    {
        Q_OBJECT
    public:
        explicit poti_group_t( QWidget* parent = nullptr );

        void set_number_of_potis( unsigned potis_count, unsigned row_count );

        poti_t& operator[]( int idx );

        const poti_t& operator[]( int idx ) const;
    };
} // namespace qt
