/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \file */

#include <QThread>
#include <QFile>
#include <QFontDatabase>
#include <CSdk/CLink.h>
#include <CSdk/CNotify.h>
#include <CSdk/CFont.h>
#include <CSdk/CNotification.h>
#include <CSdk/CSettings.h>

#include "caoslink.h"
#include "ui_caoslink.h"
#include "CaosTerminal.h"
#include "CaosInterface.h"
#include "Preferences.h"

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
    QCoreApplication::setOrganizationName("Stratify Labs, Inc");
    QCoreApplication::setOrganizationDomain("StratifyLabs.co");
    QCoreApplication::setApplicationName("Stratify Link");
    QCoreApplication::setApplicationVersion(COACTIONOS_LINK_VERSION);

    CFont::init();
    link_set_debug(100);
    ui->setupUi(this);
    ui->connectWidget->clink()->set_driver(driver.driver());
    resize( Preferences::windowSize() );

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

    //Set up the notification area
    ui->notify->setVisible(false);
    CNotify::setNotification(ui->notify);
    connect(ui->notify, SIGNAL(activated(int)), this, SLOT(notified(int)));
    connect(ui->notify, SIGNAL(dismissed(int)), this, SLOT(notificationDismissed(int)));
    CNotify::setUpdateObjects(ui->statusBar, ui->progressBar, ui->caosInterface->debug());
    qDebug("CAOS Init Complete");

    connect(ui->caosInterface, SIGNAL(installKernel()), this, SLOT(installKernelRequest()));
    connect(ui->caosInterface, SIGNAL(installApps()), this, SLOT(installAppsRequest()));


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


void Caoslink::notified(int severity){
    ui->notify->show();

    switch(severity){
    case CNotification::SEVERITY_HIGH:
        ui->caosInterface->hide();
        break;
    case CNotification::SEVERITY_MED:
        ui->caosInterface->setEnabled(false);
        break;
    }
}

void Caoslink::resizeEvent(QResizeEvent * event){
    QSize s = this->size();
    qDebug("Size is %d x %d", s.width(), s.height());
    event = 0;

    Preferences::setWindowSize(s.width(), s.height());
}

void Caoslink::notificationDismissed(int v){
    ui->notify->hide();
    ui->caosInterface->setEnabled(true);
    ui->caosInterface->show();
    v = 0;
}

void Caoslink::installKernelRequest(void){
    int i;
    //first connect
    i = 0;
    do {
        QThread::usleep(150*1000);
        ui->connectWidget->refresh();
        ui->connectWidget->connectRequested(true);
        qApp->processEvents();
        i++;
    } while(i < 10 && (ui->connectWidget->clink()->isConnected() == false) );


    //now tell the interface to install the kernel
    if( ui->connectWidget->clink()->isConnected() == true ){
        ui->caosInterface->installKernelRequest();
    } else {
        ui->connectWidget->connectRequested(false);
    }

}

void Caoslink::installAppsRequest(void){
    int i;
    //first connect
    i = 0;
    do {
        QThread::usleep(150*1000);
        ui->connectWidget->refresh();
        ui->connectWidget->connectRequested(true);
        qApp->processEvents();
        i++;
    } while(i < 10 && (ui->connectWidget->clink()->isConnected() == false) );


    //now tell the interface to install the kernel
    if( ui->connectWidget->clink()->isConnected() == true ){
        ui->caosInterface->installAppsRequest();
    } else {
        ui->connectWidget->connectRequested(false);
    }
}
