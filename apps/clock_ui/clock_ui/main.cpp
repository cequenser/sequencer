#include "backend.hpp"

#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main( int argc, char* argv[] )
{
    QCoreApplication::setAttribute( Qt::AA_EnableHighDpiScaling );
    QGuiApplication app( argc, argv );

    qmlRegisterType< qml::backend >( "Sequencer", 1, 0, "Backend" );

    QQmlApplicationEngine engine;
    engine.load( QUrl( QLatin1String( "qrc:/main.qml" ) ) );

    return app.exec();
}
