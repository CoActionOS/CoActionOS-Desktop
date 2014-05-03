/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \file */


#include <QFile>
#include <QFontDatabase>
#include <CSdk/CLink.h>
#include <CSdk/CNotify.h>
#include <CSdk/CFont.h>

#include "caoslink.h"
#include "ui_caoslink.h"
#include "CaosTerminal.h"
#include "CaosInterface.h"



/*! \details This is the main window constructor.  It initializes the GUI as
 * as well as the USB Link driver.
 *
 *
 */
Caoslink::Caoslink(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Caoslink)
{
    CNotify notify;
    notify.updateStatus("");
    qDebug("CAOS Init");
    CLink * linkDevice;
    QCoreApplication::setOrganizationName("CoActionOS, Inc");
    QCoreApplication::setOrganizationDomain("coactionos.com");
    QCoreApplication::setApplicationName("CoActionOS Link");
    QCoreApplication::setApplicationVersion(COACTIONOS_LINK_VERSION);

    CFont::init();
    link_set_debug(1);

    ui->setupUi(this);
    linkDevice = ui->connectWidget->clink();
    ui->caosInterface->setLink(linkDevice);
    ui->progressBar->setVisible(false);

    //Connect link thread
    connect(linkDevice, SIGNAL(reconnectSignal(bool)), ui->connectWidget, SLOT(connectRequested(bool)));
    ui->connectWidget->setObjectName("connectWidget");
    ui->statusBar->setObjectName("statusBar");

    QFile file(":/data/CStyle.css");
    if(file.open(QFile::ReadOnly)) {
        QString StyleSheet = QLatin1String(file.readAll());
        file.close();
#ifdef Q_WS_WIN
        file.setFileName(":/data/CStyle-win32.css");
        if( file.open(QFile::ReadOnly)){
            StyleSheet += QLatin1String(file.readAll());
            file.close();
        }
#endif

        file.setFileName(":/data/CStyle-large.css");
        if( file.open(QFile::ReadOnly)){
            StyleSheet += QLatin1String(file.readAll());
            file.close();
        }
        qApp->setStyleSheet(StyleSheet);
    }



    CNotify::setUpdateObjects(ui->statusBar, ui->progressBar);
    qDebug("CAOS Init Complete");

}

/*! \details This is the main windows destructor.  It makes sure
 * that the USB Devices are properly freed on exit as well as
 * deleting the timer used for reading data.
 *
 */
Caoslink::~Caoslink(){
    CLink * linkDevice;
    //wait for any pending operation on the device to complete
    linkDevice = ui->connectWidget->clink();
    if ( linkDevice->isConnected() == true ){
        linkDevice->exit();
    }
    delete ui;
}


