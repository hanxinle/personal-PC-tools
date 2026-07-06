#include "catclockpc.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    CatClockPC window;
    window.show();
    return app.exec();
}
