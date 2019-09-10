#pragma once

#include <QObject>

namespace qt
{
    struct signal_blocker_t
    {
        explicit signal_blocker_t( QObject& object )
            : object_{object}, last_state_{object.blockSignals( true )}
        {
        }

        ~signal_blocker_t()
        {
            object_.blockSignals( last_state_ );
        }

    private:
        QObject& object_;
        const bool last_state_;
    };
} // namespace qt
