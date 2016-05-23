/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include <QSettings>

#include "CaosTerminal.h"
#include "ui_CaosTerminal.h"

#include <CSdk/CNotify.h>
#include <CSdk/CFont.h>
#include <iface/dev/fifo.h>
#include <iface/dev/sys.h>

#include "caoslink.h"
#include "Preferences.h"


CaosTerminal::CaosTerminal(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CaosTerminal){
    qDebug("Terminal Init");
    ui->setupUi(this);

    connect(ui->terminalWidget, SIGNAL(sendText(QString)), this, SLOT(writeDevice(QString)));
    connect(&readTimer, SIGNAL(timeout()), this, SLOT(readDevice()));

    ui->runButton->setToolTip("Run most recently executed application");
    ui->runButton->setText(CFont::iconRefresh());
    ui->runButton->setObjectName("blueButton");
    ui->installButton->setToolTip("Re-install most recently installed application");
    ui->installButton->setText(CFont::iconDownloadAlt());
    ui->installButton->setObjectName("blueButton");

    inFd = -1;
    outFd = -1;
    ui->openButton->setText(CFont::iconPlay());
    ui->openButton->setObjectName("greenButton");
    ui->openButton->setToolTip("Start terminal");
    ui->openButton->setEnabled(true);
    ui->closeButton->setText(CFont::iconPause());
    ui->closeButton->setObjectName("redButton");
    ui->closeButton->setToolTip("Pause terminal");
    ui->closeButton->setEnabled(false);
    ui->preferencesButton->setObjectName("defaultButton");
    ui->preferencesButton->setText(CFont::iconPreferences());
    ui->preferencesButton->setToolTip("Show terminal preferences");

    this->linked(false);

    ui->terminalWidget->setMaxTextSize( Preferences::terminalMaximumSize().toInt() * 1024 );
    qDebug("Terminal Init Complete");
}

CaosTerminal::~CaosTerminal()
{
    //ensure terminal is closed
    if( inFd != -1 ){
        this->closeTerminal();
    }
    delete ui;
}

bool CaosTerminal::isOpen(void){
    if( inFd < 0 ){
        return false;
    }
    return true;
}

void CaosTerminal::setLink(CLink * d){
    CLinkPtr::setLink(d);
    connect(link(), SIGNAL(linked(bool)), this, SLOT(linked(bool)), Qt::DirectConnection);
}


void CaosTerminal::linked(bool arg1){

    if( link() != 0 ){
        if( link()->isBootloader() == true ){
            arg1 = false;
        }
    }

    if( arg1 == false ){
        this->closeTerminal();
        inFd = -1;
        outFd = -1;
    }
    ui->runButton->setEnabled(arg1);
    ui->installButton->setEnabled(arg1);
    ui->closeButton->setEnabled(false);
    ui->openButton->setEnabled(arg1);
}


void CaosTerminal::openTerminal(){
    QString tmpIn;
    QString tmpOut;
    CNotify notify;
    sys_attr_t attr;
    int sysFd;
    if( inFd < 0 ){

        sysFd = link()->open("/dev/sys", LINK_O_RDWR);
        if( sysFd < 0 ){
            notify.execError("Failed to load STDIO device names");
            return;
        }

        if( link()->ioctl(sysFd, I_SYS_GETATTR, &attr) < 0 ){
            link()->close(sysFd);
            notify.execError("Failed to load STDIO device names");
            return;
        }

        if( link()->close(sysFd) < 0 ){
            notify.execError("Failed to close system device");
            return;
        }

        if( attr.flags & SYS_FLAGS_STDIO_VCP ){
            //device uses a VCP for the STDIO
            CNotify::updateStatus("Use external program for terminal");
            return;
        } else if( attr.flags & SYS_FLAGS_STDIO_FIFO ) {

            tmpIn = QString(attr.stdin_name);
            tmpOut = QString(attr.stdout_name);

            if ( link()->isConnected() == false ){
                return;
            }

            if ( link()->isBootloader() > 0 ){
                return;
            }

            inFd = link()->open(tmpOut.toStdString(), LINK_O_RDWR | LINK_O_NONBLOCK);
            if( inFd < 0 ){
                notify.execError("Failed to open /dev/" + tmpIn);
                inFd = -1;
                outFd = -1;
                return;
            }

            if( tmpIn == tmpOut ){
                //if the device is the same, use the same descriptor
                outFd = inFd;
            } else {
                outFd = link()->open(tmpIn.toStdString(), LINK_O_RDWR | LINK_O_NONBLOCK);
                if( outFd < 0 ){
                    link()->close(inFd);
                    inFd = -1;
                    outFd = -1;
                    notify.execError("Failed to open /dev/" + tmpOut);
                    return;
                }
            }

            link()->ioctl(inFd, I_FIFO_SETWRITEBLOCK, (void*)1);
        }

        CNotify::updateStatus("Terminal opened");
        readTimer.setInterval(100);
        readTimer.start();

        ui->openButton->setEnabled(false);
        ui->closeButton->setEnabled(true);
        ui->terminalWidget->setLogFileName(Preferences::terminalLogFile(),
                                           Preferences::terminalAppendMode(),
                                           Preferences::terminalWarnOverwrite());
        ui->terminalWidget->opened(true);

    }
}

void CaosTerminal::closeTerminal(){
    int err;
    readTimer.stop();
    ui->terminalWidget->opened(false);
    if( inFd != -1 ){
        if( link()->isConnected() ){
            if( (err = link()->ioctl(inFd, I_FIFO_SETWRITEBLOCK, (void*)0)) < 0 ){
                qDebug("Failed to clear write block %d", err);
            }
            if( link()->close(inFd) < 0 ){
                qDebug("Failed to close input device");
            }
            if( link()->close(outFd) < 0 ){
                qDebug("failed to close output device");
            }

            CNotify::updateStatus("Terminal closed");
        } else {
            qDebug("Device is not connected");
        }
        inFd = -1;
        outFd = -1;
    }
    ui->openButton->setEnabled(true);
    ui->closeButton->setEnabled(false);
}

void CaosTerminal::readDevice(void){
    QString text;
    const int bufSize = 1024;
    char buffer[bufSize+1];
    int totalRead;
    int bytesRead;

    if( inFd < 0 ){
        return;
    }

    if ( link()->isConnected() == false ){
        readTimer.stop();
        return;
    }

    totalRead = 0;
    //read the device and add text to the terminal
    do {
        bytesRead = link()->read(inFd, buffer, bufSize);

        if( bytesRead > 0 ){
            totalRead += bytesRead;
            buffer[bytesRead] = 0;
            text = QString(buffer);
            ui->terminalWidget->receiveText(text);
            if( totalRead > 4096 ){
                break;
            }
        }

        if( bytesRead < 0 ){
            qDebug("CaosTerminal::readDevice(): Error %d %d\n", bytesRead, link_errno);
        }

    } while( bytesRead > 0);

    if( totalRead > 0 ){
        emit infoAvailable();
        readTimer.setInterval(1);
    } else {
        readTimer.setInterval(100);
    }
}

void CaosTerminal::writeDevice(QString sendString){
    int err;
    CNotify notify;
    if ( link()->isConnected() == false ){
        notify.execNotConnected();
        return;
    }

    if( outFd == -1 ){
        notify.execError("Terminal is Not Running");
        return;
    }


    qDebug("Write %s to stdin", sendString.toLocal8Bit().constData());
    sendString = sendString + "\n";
    err = link()->write(outFd,
                        sendString.toLocal8Bit().constData(),
                        sendString.size() );

    if ( err < 0 ){
        notify.execError("Failed to Send Data: " + errorMessage());
        return;
    }

}

void CaosTerminal::on_runButton_clicked()
{
    this->closeTerminal();
    emit runApplication("");
}

void CaosTerminal::on_openButton_clicked()
{
    this->openTerminal();
}

void CaosTerminal::on_closeButton_clicked()
{
    this->closeTerminal();
}

void CaosTerminal::on_preferencesButton_clicked()
{
    emit showPreferences();
}

void CaosTerminal::on_installButton_clicked()
{
    this->closeTerminal();
    emit installApplication();
}
