#pragma once

#include <QString>
#include <QWidget>

class QDial;
class QLineEdit;

namespace qt
{
    class dial_line_edit_sync : public QObject
    {
        Q_OBJECT
    public:
        dial_line_edit_sync() = default;

        dial_line_edit_sync( QDial* dial, QLineEdit* line_edit, QString prefix = "",
                             QString suffix = "", int floating_factor = 1,
                             int threshold = std::numeric_limits< int >::max() );

        void update( int value );

        void update_from_dial( int value );

        void update_from_line_edit();

        QDial& dial();

        QLineEdit& line_edit();

    signals:
        void value_changed( double );

    private:
        QDial* dial_{nullptr};
        QLineEdit* line_edit_{nullptr};
        QString prefix_{};
        QString suffix_{};
        int floating_factor_ = 1;
        int old_value_ = 0;
        int threshold_ = std::numeric_limits< int >::max();
    };
} // namespace qt
