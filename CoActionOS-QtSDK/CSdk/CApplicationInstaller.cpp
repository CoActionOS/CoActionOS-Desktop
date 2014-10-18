#include "CApplicationInstaller.h"
#include "ui_CApplicationInstaller.h"

#include <QFile>

#include "CFont.h"
#include "CNotify.h"
#include "CApplicationSettings.h"
#include "CInstaller.h"
#include "CCompress.h"
#include "CFileBrowser.h"

CApplicationInstaller::CApplicationInstaller(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CApplicationInstaller)
{
    qDebug("CApplicationInstaller Init");
    ui->setupUi(this);
    //updateNeeded = false;
    ui->uninstallButton->setToolTip("Uninstall application");

    ui->optionsRunCheckBox->setToolTip("Run program after install completes");
    ui->optionsRamCheckBox->setToolTip("Install program in RAM");
    ui->optionsStartupCheckBox->setToolTip("Run program on startup");
    ui->optionsFilesCheckBox->setToolTip("Install files with application");

    ui->dataSizeSpinBox->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->dataSizeLockCheckBox->setObjectName("lock");
    ui->dataSizeLockCheckBox->setEnabled(true);
    ui->dataSizeLockCheckBox->setToolTip("Lock/unlock size of RAM");
    ui->dataSizeSpinBox->setToolTip("Size of RAM (data/bss/stack) for application ");

    ui->uninstallButton->setObjectName("blueButton");
    ui->uninstallButton->setText(CFont::iconTrash());

    ui->installButton->setObjectName("blueButton");
    ui->installButton->setText(CFont::iconDownloadAlt());
    ui->installButton->setToolTip("Install Application");

    ui->filesInstallButton->setObjectName("blueButton");
    ui->filesInstallButton->setText(CFont::iconCopy());
    ui->filesInstallButton->setToolTip("Install associated files");

    ui->installAllCheckBox->setToolTip("Install all workspace applications");

    ui->installPath->setObjectName("unified");
    connect(ui->installPath->lineEdit(), SIGNAL(editingFinished()), this, SLOT(installPathUpdated()));

    ui->installer->setPrefix("APPLICATION");
    ui->installer->setSettings(CApplicationSettings::settings());
    ui->installer->loadPreferences();


    connect(ui->installer, SIGNAL(addProject(QString)), this, SLOT(addProject(QString)));
    connect(ui->installer, SIGNAL(projectSelected(QString)), this, SLOT(projectSelected(QString)));
    //need to load the previously loaded workspace

    connect(ui->installer, SIGNAL(projectUpdated(QString, QString, QString)),
            this, SIGNAL(projectUpdated(QString, QString, QString)));

    connect(ui->dataSizeLockCheckBox, SIGNAL(clicked(bool)), ui->dataSizeSpinBox, SLOT(setEnabled(bool)));

    ui->installer->updateProjectList();

    ui->stackLabel->setToolTip("Stack size");
    ui->stackLabel->setObjectName("warningLabel");

    connected(false);
    qDebug("CApplicationInstaller Done");
}

CApplicationInstaller::~CApplicationInstaller()
{
    delete ui;
}

void CApplicationInstaller::setLink(CLink * d){
    CLinkPtr::setLink(d);
    connect(link(), SIGNAL(linked(bool)), this, SLOT(connected(bool)));
}

void CApplicationInstaller::connected(bool arg1){

    if( link() != 0 ){
        if( link()->isBootloader() == true ){
            arg1 = false;
        }
    }
    ui->installButton->setEnabled(arg1);
    ui->uninstallButton->setEnabled(arg1);
    ui->filesInstallButton->setEnabled(arg1);
}

void CApplicationInstaller::installPathUpdated(void){
    CApplicationSettings settings( ui->installer->projectPath() );
    settings.setInstallPath(ui->installPath->lineEdit()->text());
}

void CApplicationInstaller::addProject(QString name){
    CApplicationSettings settings(ui->installer->workspace() + "/" + name);
    settings.setDataSize(4096);
    settings.setInstallPath("/app/.install");
}

void CApplicationInstaller::setProject(QString project){
    ui->installer->selectProject(project);
}

void CApplicationInstaller::setConfiguration(QString conf){
    ui->installer->selectConfiguration(conf);
}

void CApplicationInstaller::projectSelected(QString name){
    CApplicationSettings settings(ui->installer->workspace() + "/" + name);
    ui->optionsRamCheckBox->setChecked(settings.ram());
    ui->optionsRunCheckBox->setChecked(settings.run());
    ui->optionsStartupCheckBox->setChecked(settings.startup());
    ui->installPath->lineEdit()->setText(settings.installPath());
    ui->dataSizeSpinBox->setValue(settings.dataSize().toInt());
    on_optionsStartupCheckBox_clicked( ui->optionsStartupCheckBox->isChecked() );
    on_optionsRamCheckBox_clicked( ui->optionsRamCheckBox->isChecked() );
    qDebug("Project %s selected", name.toLocal8Bit().constData());
    emit selected(ui->installer->workspace() + "/" + name);
}

void CApplicationInstaller::on_installButton_clicked()
{
    int i;
    int total;
    if ( link()->connected() == false ){
        return;
    }

    CNotify::updateProgress(0, 0);
    qApp->processEvents();


    if( ui->installAllCheckBox->isChecked() ){
        total = ui->installer->count();
        for(i=0; i < total; i++){
            ui->installer->setCurrentProject(i);
            install();
        }
    } else {
        install();
    }

}

void CApplicationInstaller::installAll(void){
    ui->installAllCheckBox->setChecked();
    on_installButton_clicked();
}

void CApplicationInstaller::on_uninstallButton_clicked()
{
    CNotify notify;
    QString projectName = ui->installer->project();
    int pid;

    if( CNotify::notification() != 0 ){
        if( (pid = link()->isExecuting(projectName.toStdString())) >= 0 ){
            connect(CNotify::notification(), SIGNAL(dismissed(int)), this, SLOT(uninstallPrompt(int)));
            notify.execPrompt("Kill " + ui->installer->project() + "?");
            return;
        }
    } else {
        //ask to kill running process
        if( (pid = link()->isExecuting(projectName.toStdString())) >= 0 ){
            if( notify.execPrompt("Kill running process") == 0 ){
                CNotify::updateStatus("Uninstalled aborted");
                return;
            }

            if( link()->killPid(pid, LINK_SIGTERM) < 0 ){
                notify.execLinkError(link_errno);
                return;
            }
        }
    }

    uninstallApp();
}

void CApplicationInstaller::uninstallPrompt(int v){
    int pid;
    CNotify notify;
    disconnect(CNotify::notification(), SIGNAL(dismissed(int)), this, SLOT(uninstallPrompt(int)));
    if( v != 0 ){
        pid = link()->isExecuting(ui->installer->project().toStdString());
        if( link()->killPid(pid, LINK_SIGTERM) < 0 ){
            notify.execLinkError(link_errno);
            return;
        }
        uninstallApp();
    }

}

int CApplicationInstaller::uninstallApp(){
    CNotify notify;
    QString projectName = ui->installer->project();
    int err;
    err = 0;
    if ( ui->installPath->lineEdit()->text() == "/app/.install"){
        if ( ui->optionsRamCheckBox->isChecked() == false ){
            err = link()->unlink("/app/flash/" + projectName.toStdString());
        } else {
            err = link()->unlink("/app/ram/" + projectName.toStdString());
        }
    } else {
        err = link()->unlink((ui->installPath->lineEdit()->text() + "/" + projectName).toStdString());
    }

    if ( err != 0 ){
        notify.execLinkError(link_errno);
    } else {
        notify.updateStatus(projectName + " removed");
    }

    return err;
}

int CApplicationInstaller::install(void){
    int pid;
    CNotify notify;
    QFile binFile;
    link_appfs_file_t appfsFile;

    if( ui->installPath->lineEdit()->text() == "" ){
        notify.execError("Invalid install path");
        return -1;
    }

    if( ui->installer->project() == "" ){
        notify.execError("Project does not exist");
        return -1;
    }


    //The binary needs to be updated with the current options
    binFile.setFileName(ui->installer->configurationPath() +
                        "/" +
                        ui->installer->project());

    if ( binFile.open(QIODevice::ReadWrite) == false ){
        notify.execError("Failed to open " + binFile.fileName());
        return -1;
    }

    if ( binFile.read((char*)&appfsFile, sizeof(appfsFile)) != sizeof(appfsFile) ){
        notify.execError("Failed to read binary header for " + binFile.fileName());
        binFile.close();
        return -1;
    }

    memset(appfsFile.hdr.name, 0, LINK_NAME_MAX);
    strncpy(appfsFile.hdr.name, ui->installer->project().toLocal8Bit().constData(), LINK_NAME_MAX - 2);
    //appfsFile.hdr.uid = 0;
    //appfsFile.hdr.gid = 0;
    appfsFile.hdr.mode = 0777;

    if ( ui->optionsStartupCheckBox->isChecked() ){
        appfsFile.exec.options |= LINK_APPFS_EXEC_OPTIONS_STARTUP;
    } else {
        appfsFile.exec.options &= ~LINK_APPFS_EXEC_OPTIONS_STARTUP;
    }

    if ( ui->optionsRamCheckBox->isChecked() ){
        appfsFile.exec.options &= ~(LINK_APPFS_EXEC_OPTIONS_FLASH);
    } else {
        appfsFile.exec.options |= (LINK_APPFS_EXEC_OPTIONS_FLASH);
    }

    appfsFile.exec.ram_size = ui->dataSizeSpinBox->value();

    binFile.seek(0);

    if ( binFile.write((char*)&appfsFile, sizeof(appfsFile)) != sizeof(appfsFile) ){
        notify.execError("Failed to write header file");
        binFile.close();
        return -1;
    }

    binFile.close();

    //kill program if it is running
    qDebug("Check if %s is running\n",appfsFile.hdr.name);
    pid = link()->isExecuting(appfsFile.hdr.name);
    if( pid >= 0 ){

        if( CNotify::notification() != 0 ){
            connect(CNotify::notification(), SIGNAL(dismissed(int)), this, SLOT(installPrompt(int)));
            CNotify::notification()->setQuestion("Kill running process?");
            return 0;
        }
        qDebug("Ask to kill");
        if( notify.execPrompt("Kill running process?") == false ){
            notify.updateStatus("Install aborted");
            return -1;
        } else {
            qDebug("KILL %d", pid);
            if( link()->killPid(pid, LINK_SIGTERM) < 0 ){
                notify.execError("Failed to kill process");
                return -1;
            }
        }
    }

    //delete the existing file
    qDebug("Unlink the files");
    link()->unlink("/app/flash/" + ui->installer->project().toStdString());
    link()->unlink("/app/ram/" + ui->installer->project().toStdString());

    return installApp();
}

void CApplicationInstaller::installPrompt(int v){
    int pid;
    CNotify notify;
    disconnect(CNotify::notification(), SIGNAL(dismissed(int)), this, SLOT(installPrompt(int)));
    if( v != 0 ){
        pid = link()->isExecuting(ui->installer->project().toStdString());
        if( link()->killPid(pid, LINK_SIGTERM) < 0 ){
            notify.execLinkError(link_errno);
            return;
        }
        //delete the existing file
        qDebug("Unlink the files");
        link()->unlink("/app/flash/" + ui->installer->project().toStdString());
        link()->unlink("/app/ram/" + ui->installer->project().toStdString());
        installApp();
    }
}

QString CApplicationInstaller::projectRunPath(void){
    QString path;
    if ( ui->optionsRamCheckBox->isChecked() ){
        path = "/app/ram/";
    } else {
        path = "/app/flash/";
    }
    return path + ui->installer->project();
}

int CApplicationInstaller::installApp(){
    CNotify notify;
    QString sourcePath;
    QFile sourceFile;
    int bytesRead;
    int fd;
    appfs_installattr_t attr;
    QString installPath;
    int bytesCum;
    int bytesTotal;
    sourcePath = ui->installer->configurationPath() + "/" + ui->installer->project();

    sourceFile.setFileName(sourcePath);
    if( sourceFile.open(QIODevice::ReadOnly) == false ){
        notify.execError("Failed to open source file: " + sourcePath);
        return -1;
    }

    installPath = ui->installPath->lineEdit()->text();

    if( installPath.startsWith("/app") ){
        fd = link()->open(installPath.toStdString(), LINK_O_WRONLY);
        if( fd < 0 ){
            sourceFile.close();
            notify.execError("Failed to open install file on device");
            return -1;
        }

        attr.loc = 0;

        bytesTotal = sourceFile.size();
        bytesCum = 0;

        do {
            memset(attr.buffer, 0xFF, APPFS_PAGE_SIZE);
            bytesRead = sourceFile.read((char*)attr.buffer, APPFS_PAGE_SIZE);
            if( bytesRead > 0 ){
                attr.nbyte = bytesRead;
                bytesCum += attr.nbyte;
                if( link()->ioctl(fd, I_APPFS_INSTALL, &attr) < 0 ){
                    link()->close(fd);
                    sourceFile.close();
                    notify.execLinkError(link_errno);
                    return -1;
                }
                notify.updateProgress(bytesCum, bytesTotal);
                attr.loc += APPFS_PAGE_SIZE;
            }
        } while( bytesRead == APPFS_PAGE_SIZE );

        if( link()->close(fd) < 0 ){
            notify.execLinkError(link_errno);
        }

        sourceFile.close();

        if( ui->optionsFilesCheckBox->isChecked() == true ){
            on_filesInstallButton_clicked();
        }

        if ( ui->optionsRunCheckBox->isChecked() ){
            emit runApplication( projectRunPath() );
        }

        CNotify::updateProgress(0, 0, false);
        CNotify::updateStatus(ui->installer->project() + " install complete");

    } else {
        //copy the file to the destination directory
        CFileBrowser::copyFileToDevice(link(), sourcePath, installPath);
        CNotify::updateProgress(0, 0, false);
    }

    return 0;
}

void CApplicationInstaller::installApplication(){
    on_installButton_clicked();
}

void CApplicationInstaller::on_optionsStartupCheckBox_clicked(bool checked)
{
    CApplicationSettings settings(ui->installer->projectPath());
    if( checked == true ){
        ui->optionsRamCheckBox->setChecked(!checked);
    } else {
    }
    ui->optionsRamCheckBox->setEnabled(!checked);
    settings.setStartup(checked);
}

void CApplicationInstaller::on_optionsRamCheckBox_clicked(bool checked)
{
    CApplicationSettings settings(ui->installer->projectPath());
    if( checked == true ){
        ui->optionsStartupCheckBox->setChecked(!checked);
    }
    ui->optionsStartupCheckBox->setEnabled(!checked);
    settings.setRam(checked);
}

void CApplicationInstaller::on_optionsRunCheckBox_clicked(bool checked)
{
    CApplicationSettings settings(ui->installer->projectPath());
    settings.setRun(checked);
}

void CApplicationInstaller::on_dataSizeSpinBox_valueChanged(int arg1)
{
    CApplicationSettings settings(ui->installer->projectPath());
    settings.setDataSize(arg1);


    if( parseDataSize() == true ){
        //calculate the data size
        int stackSize;
        stackSize = arg1 - (dataSize + bssSize);


        ui->stackLabel->setEnabled(true);

        if( stackSize < 1024 ){
            ui->stackLabel->setEnabled(false);
            if( stackSize < 0 ){
                stackSize = 0;
            }
            ui->stackLabel->setText(CFont::fontAwesome(CFont::iconWarningSign()) + " " + QString::number(stackSize));
        } else {
            ui->stackLabel->setText(CFont::fontAwesome(CFont::iconStack()) + " " + QString::number(stackSize));
        }
    } else {
        ui->stackLabel->setEnabled(false);
        ui->stackLabel->setText(CFont::fontAwesome(CFont::iconQuestionSign()));
    }

}

bool CApplicationInstaller::parseDataSize(void){
    QString sourcePath;
    QString head;
    QString data;
    int nbyte;
    char buffer[2048];
    QStringList list;

    sourcePath = ui->installer->configurationPath() + "/size.txt";

    QFile sourceFile(sourcePath);

    if( sourceFile.open(QFile::ReadOnly | QFile::Text) == false ){
        //the information is not available
        return false;
    }

    //first line is the heading
    nbyte = sourceFile.readLine(buffer, 2048);
    if( nbyte > 0 ){
        head.append(QString(buffer));
    }
    nbyte = sourceFile.readLine(buffer, 2048);
    if( nbyte > 0 ){
        data.append(QString(buffer));
    } else {
        return false;
    }

    sourceFile.close();


    //now parse data to get the size of the RAM -- data and bss sections (app will also need room for the stack)
    list = data.split(' ', QString::SkipEmptyParts);

    bssSize = list.at(1).toInt();
    dataSize = list.at(2).toInt();

    return true;

}


void CApplicationInstaller::on_filesInstallButton_clicked()
{
    bool ret;
    QString targetDir;
    QDir dir(ui->installer->projectPath() + "/data");
    QStringList dirs;
    int i;
    CNotify notify;

    //copy the folder structure in project/data to the device
    dirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for(i=0; i < dirs.size(); i++){
        if( dirs.at(i) == "flash" ){
            targetDir = "/app/flash";
        } else {
            targetDir = "/" + dirs.at(i);
        }
        ret = CFileBrowser::copyDirToDevice(link(), dir.path() + "/" + dirs.at(i), targetDir);
        if( ret == false ){
            qDebug("exec error");
            notify.execError("Failed to copy files in data/" + dirs.at(i));
            return;
        }
    }

    CNotify::updateStatus("File install complete");

}

void CApplicationInstaller::resizeEvent(QResizeEvent * event){
    if( size().width() < 575 ){
        ui->optionsLabel->hide();
        ui->installPathLabel->hide();
        ui->installer->showLabels(false);
    } else {
        ui->installPathLabel->show();
        ui->optionsLabel->show();
        ui->installer->showLabels(true);
    }
    event = 0;
}
