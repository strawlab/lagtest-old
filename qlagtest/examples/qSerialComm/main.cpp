#include <QCoreApplication>
#include "mainapp.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    
    mainApp app;

    app.start();

    return a.exec();
}
