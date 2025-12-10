#include "lex_clock.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    lex_clock window;
    window.show();
    return app.exec();
}
