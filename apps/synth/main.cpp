#include "synth.hpp"

#include <QApplication>

void callback( double, std::vector< unsigned char >*, void* );

synth& get_synth()
{
    static synth syn( nullptr, &callback );
    return syn;
}

void callback( double dt, std::vector< unsigned char >* message, void* )
{
    if ( message == nullptr || message->empty() )
    {
        return;
    }

    if ( message->size() == 1u )
    {
        get_synth().backend().receive_message< 1 >( dt, {std::byte{( *message )[ 0 ]}} );
    }
    if ( message->size() == 3u )
    {
        get_synth().backend().receive_message< 3 >( dt, {std::byte{( *message )[ 0 ]},
                                                         std::byte{( *message )[ 1 ]},
                                                         std::byte{( *message )[ 2 ]}} );
    }
}

int main( int argc, char* argv[] )
{
    QApplication a( argc, argv );
    get_synth().show();

    return a.exec();
}
