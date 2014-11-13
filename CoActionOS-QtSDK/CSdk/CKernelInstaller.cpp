#include "CKernelInstaller.h"
#include "ui_CKernelInstaller.h"

#include <QTimer>
#include <QFile>
#include <dev/mem.h>
#include <applib/Device.hpp>

#include "CKernelSettings.h"
#include "CFont.h"
#include "CCompress.h"
#include "CNotify.h"

static bool updateProgress(int prog, int max){
    CNotify::updateProgress(prog,max);
    qApp->processEvents();
    return false; //do not abort
}

CKernelInstaller::CKernelInstaller(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CKernelInstaller)
{
    ui->setupUi(this);

    ui->installButton->setObjectName("blueButton");
    ui->installButton->setText(CFont::iconDownloadAlt());
    ui->installButton->setToolTip("Install kernel on device");

    ui->uploadButton->setObjectName("blueButton");
    ui->uploadButton->setText(CFont::iconUploadAlt());
    ui->uploadButton->setToolTip("Load kernel to file");
    ui->uploadButton->setVisible(false);

    ui->verifyInstallCheckBox->setToolTip("Verify kernel after install");

    ui->installer->setSettings(CKernelSettings::settings());
    ui->installer->setPrefix("KERNEL");
    ui->installer->loadPreferences();

    connect(ui->installer, SIGNAL(addProject(QString)), this, SLOT(addProject(QString)));
    connect(ui->installer, SIGNAL(projectSelected(QString)), this, SLOT(projectSelected(QString)));
    connect(ui->installer, SIGNAL(projectUpdated(QString,QString,QString)),
            this, SIGNAL(projectUpdated(QString,QString,QString)));

    ui->installer->updateProjectList();

    loadPreferences();

    connected(false);
}

CKernelInstaller::~CKernelInstaller()
{
    savePreferences();
    delete ui;
}

void CKernelInstaller::setLink(CLink *d){
    CLinkPtr::setLink(d);
    connect(link(), SIGNAL(linked(bool)), this, SLOT(connected(bool)));
}


void CKernelInstaller::savePreferences(){
    CSettings s(CSettings::userScope());
    s.setKey("KEY_KERNELVERIFYINSTALL", ui->verifyInstallCheckBox->isChecked());
}

void CKernelInstaller::loadPreferences(){
    CSettings s(CSettings::userScope());
    ui->verifyInstallCheckBox->setChecked( s.getBoolKey("KEY_KERNELVERIFYINSTALL") );
}

void CKernelInstaller::connected(bool arg1){
    ui->installButton->setEnabled(arg1);
}

void CKernelInstaller::addProject(QString name){
    CKernelSettings settings(ui->installer->workspace() + "/" + name);
    settings.setVerifyInstall(ui->verifyInstallCheckBox->isChecked());
}

void CKernelInstaller::projectSelected(QString name){
    CKernelSettings settings(ui->installer->workspace() + "/" + name);
    ui->verifyInstallCheckBox->setChecked( settings.verifyInstall() );
}

void CKernelInstaller::setProject(QString project){
    ui->installer->selectProject(project);
}

void CKernelInstaller::setConfiguration(QString conf){
    ui->installer->selectConfiguration(conf);
}

void CKernelInstaller::on_installButton_clicked()
{
    QTimer resetTimer;
    QString message;
    QString projectName;
    CNotify notify;
    int interval;
    int i;

    projectName = ui->installer->project();

    if ( link()->isConnected() == false ){
        //setErrorMessage("Connect to CoActionOS Before Continuing\n" +
        //"(Manually Invoke the Bootloader if necessary)");
        return;
    } else {

        ui->installButton->setEnabled(false);

        if ( link()->isBootloader() == false ){
            CNotify::updateStatus("Waiting for bootloader");
            CNotify::updateProgress(0, 0);

            qDebug("emit about to reset");
            emit aboutToReset(); //connected to close terminal

            interval = 500;

            resetTimer.setInterval(interval);
            resetTimer.setSingleShot(true);
            resetTimer.start();
            while( resetTimer.isActive() ){
                qApp->processEvents();  //this updates the resetTimer.isActive() return value
            }

            qDebug("now reset to bootloader");
            //Invoke the bootloader
            if ( link()->resetBootloader() == 0 ){

                //need to wait until the device is available
                resetTimer.setInterval(interval);
                resetTimer.setSingleShot(true);

                for(i=0; i < 60; i++){

                    resetTimer.start();
                    while( resetTimer.isActive() ){
                        qApp->processEvents();
                    }

                    if ( link()->getNumDevices() > 0 ){
                        //connect to last known serial number
                        link()->reinit(); //suppress any error messages
                    }

                    resetTimer.start();
                    while( resetTimer.isActive() ){
                        qApp->processEvents();
                    }

                    if ( link()->isConnected() ){
                        break;
                    }


                }

                if ( i == 60 ){
                    message = "Bootloader did not load;\n";
                    message += "try invoking manually.";
                    notify.execError(message);
                    return;
                }

            } else {
                notify.execError("Failed to start bootloader");
                return;
            }
        }
    }


    ui->installButton->setEnabled(false);
    qApp->processEvents();

    CNotify::updateStatus("Installing kernel");
    CNotify::updateProgress(0, 0, false);

    if( link()->updateOS((ui->installer->configurationPath() + "/" + projectName + ".bin").toStdString(),
                         ui->verifyInstallCheckBox->isChecked(), updateProgress) < 0 ){
        notify.execError(errorMessage());
    }

    CNotify::updateProgress(0, 0, false);

    link()->reset();

    if( ui->installAppsCheckbox->isChecked() ){
        emit installApps();
    }

    return;
}

void CKernelInstaller::installKernel(void){
    on_installButton_clicked();
}


void CKernelInstaller::on_verifyInstallCheckBox_clicked(bool checked)
{
    CKernelSettings settings(ui->installer->projectPath());
    settings.setVerifyInstall(checked);
}

void CKernelInstaller::resizeEvent(QResizeEvent * event){
    if( size().width() < 575 ){
        ui->installer->showLabels(false);
    } else {
        ui->installer->showLabels(true);
    }
    event = 0;
}


void CKernelInstaller::on_uploadButton_clicked()
{

    Device mem;
    int offset;
    int ret;
    char buffer[1024];
    mem_attr_t attr;

    mem.sethandle(link()->handle());

    if( mem.open("/dev/mem0", LINK_O_RDWR) < 0 ){
        CNotify::updateStatus("Failed to open /dev/mem");
        return;
    }

    if( mem.ioctl(I_MEM_GETATTR, &attr) < 0 ){
        CNotify::updateStatus("Failed to get mem attr");
        mem.close();
        return;
    }

    //open /dev/mem and copy data to a file in the project folder
    QFile f(ui->installer->configurationPath() + "/kernel-image.bin");

    if( f.open(QFile::WriteOnly | QFile::Truncate | QFile::Append) == false ){
        CNotify::updateStatus("Failed to open " + f.fileName());
        mem.close();
        return;
    }

    offset = 0;
    do {

        ret = mem.read(offset, buffer, 1024);
        if( ret > 0 ){
            f.write(buffer, ret);
            offset += ret;
        }

        CNotify::updateProgress(offset, attr.flash_size, true);

    } while(ret > 0);

    f.close();
    mem.close();

    CNotify::updateProgress(0, 0, false);

}
