#-------------------------------------------------
#
# Project created by QtCreator 2019-09-09T22:55:24
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = midi_sequencer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++17

QMAKE_CXX = clang++-6.0

SOURCES += \
        main.cpp \
        midi_sequencer.cpp \
        poti.cpp \
        poti_group.cpp \
        scale_dialog.cpp \
        track.cpp \
        util.cpp

HEADERS += \
        midi_sequencer.hpp \
        poti.hpp \
        poti_group.hpp \
        scale_dialog.hpp \
        signal_blocker.hpp \
        track.hpp \
        util.hpp

FORMS += \
        midi_sequencer.ui

INCLUDEPATH += $$PWD/../../../include \
    $$PWD/../../../external/repo/rtmidi \
    $$PWD/../../../external/repo/portaudio/include

LIBS += -lrtmidi -lportaudio
LIBS += -L"$$PWD/../../../build4/lib"

device_specs.commands = $(COPY_DIR) $$PWD/../../../device_spec $$OUT_PWD
midi_sequencer.depends = $(midi_sequencer) device_specs
export(midi_sequencer.depends)
export(device_specs.commands)
QMAKE_EXTRA_TARGETS += midi_sequencer device_specs

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
