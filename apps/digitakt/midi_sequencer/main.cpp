#include "midi_sequencer.hpp"

#include <QApplication>
#include <cstddef>
#include <iostream>

void callback( double, std::vector< unsigned char >*, void* );

midi_sequencer& get_sequencer()
{
    static midi_sequencer sequencer( nullptr, &callback );
    return sequencer;
}

void callback( double timedelta, std::vector< unsigned char >* message, void* )
{
    if ( message == nullptr || message->empty() )
    {
        return;
    }
    if ( message->size() == 1u )
    {
        get_sequencer().backend().receive_clock_message( {std::byte{( *message )[ 0 ]}},
                                                         get_sequencer().backend().sender() );
    }
}

int main( int argc, char* argv[] )
{
    QApplication a( argc, argv );
    get_sequencer().show();

    return a.exec();
}
