#pragma once

#include <QGroupBox>
#include <string>
#include <vector>

class QDial;
class QLineEdit;
class QString;

namespace qt
{
    class poti_t : public QGroupBox
    {
        Q_OBJECT
    public:
        explicit poti_t( QWidget* parent = nullptr );

        void update( int value );

        void set_suffix( const QString& suffix );
        void set_decimals( int decimals );

        QDial& dial();

        QLineEdit& line_edit();

        void set_str_map( const std::vector< std::string >& str_map );

        void set_map( const std::vector< int >& map );

    signals:
        void value_changed( int );

    private:
        void update_from_dial( int value );

        void update_from_line_edit();

        QDial* dial_{nullptr};
        QLineEdit* line_edit_{nullptr};
        QString suffix_{};
        int decimals_ = 0;
        int old_value_ = 0;
        std::vector< std::string > str_map_;
        std::vector< int > map_;
    };
} // namespace qt
