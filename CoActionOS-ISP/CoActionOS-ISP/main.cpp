#include <QtWidgets/QApplication>
#include "CaosIsp.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CaosIsp w;
    w.show();
    
    return a.exec();
}
