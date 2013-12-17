#include "CApplicationInstaller.h"
#include "ui_CApplicationInstaller.h"

#include <QFile>

#include "CFont.h"
#include "CNotify.h"
#include "CApplicationSettings.h"
#include "CInstaller.h"
#include "CCompress.h"

CApplicationInstaller::CApplicationInstaller(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::CApplicationInstaller)
{
  CSettings settings(QSettings::UserScope, "CoActionOS-QtSDK");

  qDebug("CApplicationInstaller Init");
  ui->setupUi(this);
  //loadSettings();
  //updateNeeded = false;
  ui->uninstallButton->setToolTip("Uninstall appication");

  ui->optionsRunCheckBox->setToolTip("Run program after install completes");
  ui->optionsRamCheckBox->setToolTip("Install program in RAM");
  ui->optionsStartupCheckBox->setToolTip("Run program on startup");

  ui->dataSizeSpinBox->setAttribute(Qt::WA_MacShowFocusRect, 0);
  ui->dataSizeLockCheckBox->setObjectName("lock");
  ui->dataSizeLockCheckBox->setEnabled(true);
  ui->dataSizeLockCheckBox->setToolTip("Lock/unlock program data size");
  ui->dataSizeSpinBox->setToolTip("Size for program data");

  ui->uninstallButton->setObjectName("blueButton");
  ui->uninstallButton->setText(CFont::iconTrash());

  ui->installButton->setObjectName("blueButton");
  ui->installButton->setText(CFont::iconDownloadAlt());
  ui->installButton->setToolTip("Install Application");

  ui->installPath->setObjectName("unified");
  connect(ui->installPath->lineEdit(), SIGNAL(editingFinished()), this, SLOT(installPathUpdated()));

  ui->installer->setSettings(CApplicationSettings::settings());
  ui->installer->setWorkspace(settings.getStringKey("KEY_" + CApplicationSettings::settings()));

  connect(ui->installer, SIGNAL(addProject(QString)), this, SLOT(addProject(QString)));
  connect(ui->installer, SIGNAL(projectSelected(QString)), this, SLOT(projectSelected(QString)));
  //need to load the previously loaded workspace

  ui->installer->updateProjectList();

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
  ui->installButton->setEnabled(arg1);
  ui->uninstallButton->setEnabled(arg1);
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

void CApplicationInstaller::projectSelected(QString name){
  CApplicationSettings settings(ui->installer->workspace() + "/" + name);
  ui->optionsRamCheckBox->setChecked(settings.ram());
  ui->optionsRunCheckBox->setChecked(settings.run());
  ui->optionsStartupCheckBox->setChecked(settings.startup());
  ui->installPath->lineEdit()->setText(settings.installPath());
  ui->dataSizeSpinBox->setValue(settings.dataSize().toInt());
  on_optionsStartupCheckBox_clicked( ui->optionsStartupCheckBox->isChecked() );
  on_optionsRamCheckBox_clicked( ui->optionsRamCheckBox->isChecked() );
}

void CApplicationInstaller::on_installButton_clicked()
{
  if ( link()->connected() == false ){
      return;
    }

  CNotify::updateProgress(0, 0);
  qApp->processEvents();
  if ( install() != 0 ){
      return;
    } else {
      if ( ui->optionsRunCheckBox->isChecked() ){
          emit runApplication( projectRunPath() );
        }
    }
  CNotify::updateProgress(0, 0, false);
  CNotify::updateStatus("Install complete");
}

void CApplicationInstaller::on_uninstallButton_clicked()
{
  CNotify notify;
  QString projectName = ui->installer->project();
  int err;
  int pid;
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

  //uninstall project
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
}
