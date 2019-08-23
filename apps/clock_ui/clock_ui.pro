QT += qml quick

CONFIG += c++17

QMAKE_CXX = clang++-6.0

SOURCES += main.cpp \
    backend.cpp

INCLUDEPATH += /home/lars/Projects/sequencer/include \
    /home/lars/Projects/sequencer/external/repo/install/rtmidi/include \
    /home/lars/Projects/sequencer/external/repo/install/portaudio/include

LIBS += -lrtmidi -lportaudio
LIBS += -L"/home/lars/Projects/sequencer/external/repo/install/rtmidi/lib"
LIBS += -L"/home/lars/Projects/sequencer/external/repo/install/portaudio/lib"

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    backend.hpp
