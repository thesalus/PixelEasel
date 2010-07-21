#include <QtGui/QApplication>
#include "PixelEasel.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PixelEasel w;
    w.show();
    return a.exec();
}
