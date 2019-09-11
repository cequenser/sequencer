#include "util.hpp"

#include <QApplication>

namespace qt
{
    bool use_secondary_function() noexcept
    {
        return QApplication::keyboardModifiers().testFlag( Qt::ControlModifier );
    }
} // namespace qt
