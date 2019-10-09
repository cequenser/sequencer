#-------------------------------------------------
#
# Project created by QtCreator 2019-10-02T17:28:55
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = synth
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
        chain.cpp \
        delay.cpp \
        envelope.cpp \
        main.cpp \
        osc.cpp \
        osc_freq.cpp \
        poti.cpp \
        synth.cpp

HEADERS += \
        chain.hpp \
        delay.hpp \
        envelope.hpp \
        osc.hpp \
        osc_freq.hpp \
        poti.hpp \
        signal_blocker.hpp  \
        synth.hpp

FORMS += \
        synth.ui

INCLUDEPATH += $$PWD/../../include \
    $$PWD/../../external/repo/rtmidi \
    $$PWD/../../external/repo/portaudio/include

LIBS += -lrtmidi -lportaudio -lstdc++fs
LIBS += -L"$$PWD/../../build4/lib"

impulse_responses.commands = $(COPY_DIR) $$PWD/impulse_responses $$OUT_PWD
synth.depends = $(synth) impulse_responses
export(synth.depends)
export(impulse_responses.commands)
QMAKE_EXTRA_TARGETS += synth impulse_responses

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
