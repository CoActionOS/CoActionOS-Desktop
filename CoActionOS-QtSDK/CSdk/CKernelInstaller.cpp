#include "CKernelInstaller.h"
#include "ui_CKernelInstaller.h"

#include <QTimer>

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
  CSettings settings(QSettings::UserScope, "CoActionOS-QtSDK");
  ui->setupUi(this);

  ui->installButton->setObjectName("blueButton");
  ui->installButton->setText(CFont::iconDownloadAlt());
  ui->installButton->setToolTip("Install kernel on device");
  ui->verifyInstallCheckBox->setToolTip("Verify kernel after install");

  ui->installer->setSettings(CKernelSettings::settings());
  ui->installer->setWorkspace(settings.getStringKey("KEY_" + CKernelSettings::settings()));

  connect(ui->installer, SIGNAL(addProject(QString)), this, SLOT(addProject(QString)));
  connect(ui->installer, SIGNAL(projectSelected(QString)), this, SLOT(projectSelected(QString)));


  ui->installer->updateProjectList();

  connected(false);
}

CKernelInstaller::~CKernelInstaller()
{
  delete ui;
}

void CKernelInstaller::setLink(CLink *d){
  CLinkPtr::setLink(d);
  connect(link(), SIGNAL(linked(bool)), this, SLOT(connected(bool)));
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

  return;
}


void CKernelInstaller::on_verifyInstallCheckBox_clicked(bool checked)
{
  CKernelSettings settings(ui->installer->projectPath());
  settings.setVerifyInstall(checked);
}

