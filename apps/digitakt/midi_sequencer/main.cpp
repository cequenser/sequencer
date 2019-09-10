#include "midi_sequencer.hpp"

#include <QApplication>

int main( int argc, char* argv[] )
{
    QApplication a( argc, argv );
    midi_sequencer w;
    w.show();

    return a.exec();
}
