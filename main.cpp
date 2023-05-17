
#include "dance.h"

#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    dance w;
    w.show();
    return a.exec();
}
