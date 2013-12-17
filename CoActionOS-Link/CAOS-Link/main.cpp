/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include <QtWidgets/QApplication>
#include <QFile>
#include "caoslink.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Caoslink w;
    w.show();
    return a.exec();
}
