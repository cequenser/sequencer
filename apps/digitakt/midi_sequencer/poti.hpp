#pragma once

#include <QGroupBox>

class QDial;
class QLineEdit;
class QString;

namespace qt
{
    class poti_t : public QGroupBox
    {
        Q_OBJECT
    public:
        poti_t() = default;

        explicit poti_t( QGroupBox* box );

        void update( int value );

        void set_suffix( const QString& suffix );
        void set_floating_factor( int factor );
        void set_threshold( int threshold );

        QDial& dial();

        QLineEdit& line_edit();

    signals:
        void value_changed( double );

    private:
        void update_from_dial( int value );

        void update_from_line_edit();

        QDial* dial_{nullptr};
        QLineEdit* line_edit_{nullptr};
        QString suffix_{};
        int floating_factor_ = 1;
        int old_value_ = 0;
        int threshold_ = std::numeric_limits< int >::max();
    };
} // namespace qt
