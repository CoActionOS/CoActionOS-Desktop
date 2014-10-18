/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include <CSdk/CFont.h>

#include "CaosInterface.h"
#include "Preferences.h"
#include "ui_CaosInterface.h"
#include "caoslink.h"
#include <CSdk/CNotify.h>
#include <CSdk/CFont.h>
#include <QFileInfo>

CaosInterface::CaosInterface(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CaosInterface){
    qDebug("CAOS Interface Init");

    //this must be set before the UI is setup
    ui->setupUi(this);
    connect(ui->fileBrowserTab, SIGNAL(runApplication(QString)), this, SLOT(runApplicationInTerminal(QString)));
    connect(ui->applicationTab, SIGNAL(runApplication(QString)), this, SLOT(runApplicationInTerminal(QString)));
    connect(ui->terminalTab, SIGNAL(installApplication()), ui->applicationTab, SLOT(installApplication()));
    connect(ui->terminalTab, SIGNAL(runApplication(QString)), this, SLOT(runApplicationInTerminal(QString)));
    connect(ui->kernelTab, SIGNAL(aboutToReset()), ui->terminalTab, SLOT(closeTerminal()), Qt::DirectConnection);

    ui->tabs->setTabText(APPLICATION_TAB_INDEX, CFont::iconApplication());
    ui->tabs->setTabToolTip(APPLICATION_TAB_INDEX, "Application");
    ui->tabs->setTabText(KERNEL_TAB_INDEX, CFont::iconKernel());
    ui->tabs->setTabToolTip(KERNEL_TAB_INDEX, "Kernel");
    ui->tabs->setTabText(ISP_TAB_INDEX, CFont::iconDownloadAlt());
    ui->tabs->setTabToolTip(ISP_TAB_INDEX, "ISP");
    ui->tabs->setTabText(FILEBROWSER_TAB_INDEX, CFont::iconFolderClose());
    ui->tabs->setTabToolTip(FILEBROWSER_TAB_INDEX, "Filebrowser");
    ui->tabs->setTabText(TERMINAL_TAB_INDEX, CFont::iconTerminal());
    ui->tabs->setTabToolTip(TERMINAL_TAB_INDEX, "Terminal");
    ui->tabs->setTabText(DEBUG_TAB_INDEX, CFont::iconDebug());
    ui->tabs->setTabToolTip(DEBUG_TAB_INDEX, "Debug");
    ui->tabs->setTabText(MONITOR_TAB_INDEX, CFont::iconMonitor());
    ui->tabs->setTabToolTip(MONITOR_TAB_INDEX, "Monitor");
    ui->tabs->setTabText(PREFERENCES_TAB_INDEX, CFont::iconPreferences());
    ui->tabs->setTabToolTip(PREFERENCES_TAB_INDEX, "Preferences");
    ui->tabs->setTabText(ABOUT_TAB_INDEX, CFont::iconAbout());
    ui->tabs->setTabToolTip(ABOUT_TAB_INDEX, "About");

    connect(ui->terminalTab, SIGNAL(infoAvailable()), this, SLOT(terminalInfoAvailable()));
    connect(ui->terminalTab, SIGNAL(showPreferences()), this, SLOT(showTerminalPreferences()));
    connect(ui->applicationTab, SIGNAL(projectUpdated(QString, QString, QString)),
            this, SLOT(applicationUpdated(QString, QString, QString)) );
    connect(ui->kernelTab, SIGNAL(projectUpdated(QString, QString, QString)),
            this, SLOT(kernelUpdated(QString, QString, QString)) );
    connect(ui->ispTab, SIGNAL(installKernel()), this, SIGNAL(installKernel()));
    connect(ui->kernelTab, SIGNAL(installApps()), this, SIGNAL(installApps()));

    infoTimer.setInterval(1000);
    consoleInfo = false;
    terminalInfo = false;
    connect(&infoTimer, SIGNAL(timeout()), this, SLOT(updateInfo()));
    infoTimer.start();

    ui->debugTab->enableMessageHandler();
    qDebug("CAOS Interface Init Complete");

    ui->applicationTab->setProject( Preferences::applicationProject() );
    ui->applicationTab->setConfiguration( Preferences::applicationConfiguration() );

    ui->kernelTab->setProject( Preferences::kernelProject() );
    ui->kernelTab->setConfiguration( Preferences::kernelConfiguration() );

}

CaosInterface::~CaosInterface(){
    delete ui;
}

void CaosInterface::refresh(void){}

CDebug * CaosInterface::debug(){
    return ui->debugTab;
}

void CaosInterface::kernelUpdated(QString workspace, QString project, QString conf){
    Preferences::setkernelProject(project);
    Preferences::setkernelConfiguration(conf);
    ui->debugTab->currentKernelUpdated(workspace, project, conf);
}

void CaosInterface::applicationUpdated(QString workspace, QString project, QString conf){
    Preferences::setApplicationProject(project);
    Preferences::setApplicationConfiguration(conf);
    ui->debugTab->currentAppUpdated(workspace, project, conf);
}


void CaosInterface::setLink(CLink * d){
    CLinkPtr::setLink(d);

    connect(link(), SIGNAL(linked(bool)), this, SLOT(linked(bool)), Qt::DirectConnection);
    ui->applicationTab->setLink(d);
    ui->kernelTab->setLink(d);
    ui->fileBrowserTab->setLink(d);
    ui->terminalTab->setLink(d);
    ui->monitorTab->setLink(d);
    ui->preferencesTab->setLink(d);
    ui->debugTab->setLink(d);
    ui->aboutTab->setLink(d);
    ui->tabs->setCurrentIndex( Preferences::currentTab().toInt() );
    on_tabs_currentChanged( ui->tabs->currentIndex() );
}

void CaosInterface::installKernelRequest(void){
    ui->tabs->setCurrentIndex(KERNEL_TAB_INDEX);
    ui->kernelTab->installKernel();
}


void CaosInterface::installAppsRequest(void){
    ui->tabs->setCurrentIndex(APPLICATION_TAB_INDEX);
    ui->applicationTab->installAll();
}

void CaosInterface::showPreferences(void){
    ui->tabs->setCurrentIndex(PREFERENCES_TAB_INDEX);
}

void CaosInterface::showKernelPreferences(void){
    ui->tabs->setCurrentIndex(PREFERENCES_TAB_INDEX);
}

void CaosInterface::showTerminalPreferences(void){
    ui->tabs->setCurrentIndex(PREFERENCES_TAB_INDEX);
}

void CaosInterface::showApplicationPreferences(void){
    ui->tabs->setCurrentIndex(PREFERENCES_TAB_INDEX);
}

void CaosInterface::showKernel(void){
    ui->tabs->setCurrentIndex(KERNEL_TAB_INDEX);
}


void CaosInterface::updateInfo(void){

    if( terminalInfo == true ){
        if( ui->tabs->tabText(TERMINAL_TAB_INDEX) == CFont::iconTerminal() ){
            ui->tabs->setTabText(TERMINAL_TAB_INDEX, CFont::iconInfoSign());
        } else {
            ui->tabs->setTabText(TERMINAL_TAB_INDEX, CFont::iconTerminal());
        }

        if( ui->tabs->currentIndex() == TERMINAL_TAB_INDEX ){
            terminalInfo = false;
            ui->tabs->setTabText(TERMINAL_TAB_INDEX, CFont::iconTerminal());
        }
    }
}


void CaosInterface::consoleInfoAvailable(void){
    consoleInfo = true;
}

void CaosInterface::terminalInfoAvailable(void){
    terminalInfo = true;
}

void  CaosInterface::checkCloseTerminal(void){
    if( ui->terminalTab->isOpen() ){
        if( ui->monitorTab->isRunningUser() == false ){
            ui->terminalTab->closeTerminal();
        }
    }
}

void  CaosInterface::checkOpenTerminal(void){
    if( ui->terminalTab->isOpen() == false ){
        if( ui->monitorTab->isRunningUser() == true ){
            ui->terminalTab->openTerminal();
        }
    }
}

void CaosInterface::on_tabs_currentChanged(int index){
    ui->monitorTab->stopTimer();
    if( index == APPLICATION_TAB_INDEX ){
        //ui->tabs->setTabText(LABEL_TAB_INDEX, "Application");
        checkCloseTerminal();
    }
    if( index == KERNEL_TAB_INDEX ){
        //ui->tabs->setTabText(LABEL_TAB_INDEX, "Kernel");
    }
    if( index == FILEBROWSER_TAB_INDEX){
        //ui->tabs->setTabText(LABEL_TAB_INDEX, "Filesystem");
        ui->terminalTab->closeTerminal();
    }
    if( index == TERMINAL_TAB_INDEX ){
        terminalInfo = false;
        ui->tabs->setTabText(TERMINAL_TAB_INDEX, CFont::iconTerminal());
        //ui->tabs->setTabText(LABEL_TAB_INDEX, "Terminal");
    }
    if( index == MONITOR_TAB_INDEX ){
        if( link()->isConnected() == true ){
            ui->monitorTab->startTimer();
        }
        checkCloseTerminal();
        //ui->tabs->setTabText(LABEL_TAB_INDEX, "Monitor");
    }

    if( index ==  ABOUT_TAB_INDEX ){
        //ui->tabs->setTabText(LABEL_TAB_INDEX, "About");
        checkCloseTerminal();
    }

    if( index == PREFERENCES_TAB_INDEX ){
        //ui->tabs->setTabText(LABEL_TAB_INDEX, "Preferences");
        checkCloseTerminal();
    }

    Preferences::setCurrenTab(QString::number(index));

}

void CaosInterface::linked(bool connected){
    ui->monitorTab->stopTimer();
    if ( connected == true ){
        if ( ui->tabs->currentIndex() == MONITOR_TAB_INDEX ){
            ui->monitorTab->startTimer();
        } else if ( ui->tabs->currentIndex() == TERMINAL_TAB_INDEX){
            qDebug("OPEN TERMINAL");
            checkOpenTerminal();
        }
        ui->tabs->setTabText(FILEBROWSER_TAB_INDEX, CFont::iconFolderOpen());

    } else {
        ui->tabs->setTabText(FILEBROWSER_TAB_INDEX, CFont::iconFolderClose());
    }
}

void CaosInterface::runApplicationInTerminal(QString path){
    int err;
    CNotify notify;
    QFileInfo info;
    int pid;


    if( path == "" ){
        path = lastApp;
        qDebug("App was %s", path.toLocal8Bit().constData());
    }

    if ( link()->isConnected() == false ){
        notify.execError("Device not Connected");
        return;
    }

    if ( path == "" ){
        if( notify.execPrompt("Nothing to Run. Use Filesystem to execute a program?") == true ){
            ui->tabs->setCurrentIndex( FILEBROWSER_TAB_INDEX );
        }
        return;
    }

    info.setFile(path);
    if( (pid = link()->isExecuting(info.fileName().toStdString())) >= 0 ){
        if( notify.execPrompt("Kill and Re-run?") == false ){
            return;
        } else {
            if( link()->killPid(pid, LINK_SIGTERM) < 0 ){
                notify.execError("Failed to Kill program");
                return;
            }
        }
    }

    ui->terminalTab->openTerminal();
    err = link()->runApp( path.toStdString() );


    if ( err >= 0 ){
        ui->tabs->setCurrentIndex( TERMINAL_TAB_INDEX );
        lastApp = path;
    } else {
        notify.execError(errorMessage());
    }

    //open trace on newly run program
    ui->debugTab->startPidTrace(err);

}

