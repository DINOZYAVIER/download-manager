#include "precompiled.h"
#include "mainwindow.h"

int main( int argc, char* argv[] )
{
    QApplication app( argc, argv );
    MainWindow wnd;
    QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuBar);
    wnd.show();
    return app.exec();
}
