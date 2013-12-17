#include "CaosIsp.h"
#include "ui_CaosIsp.h"

#include <QFile>
#include <CSdk/CFont.h>
#include <CSdk/CNotify.h>

#include "Settings.h"

CaosIsp::CaosIsp(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::CaosIsp)
{

  Settings settings(Settings::global());
  CLink * device;
  CFont::init();

  QCoreApplication::setOrganizationName("CoActionOS, Inc");
  QCoreApplication::setOrganizationDomain("coactionos.com");
  QCoreApplication::setApplicationName("CoActionOS-ISP");
  QCoreApplication::setApplicationVersion(COACTIONOS_ISP_VERSION);

  ui->setupUi(this);
  CNotify::setUpdateObjects(ui->statusBar, 0);

  link_set_debug(1);

  device = ui->connectWidget->clink();

  QFile file(":/data/CStyle.css");
  if( file.open(QFile::ReadOnly)) {
      QString StyleSheet = QLatin1String(file.readAll());
      qApp->setStyleSheet(StyleSheet);
    }

  ui->tabWidget->setTabText(TAB_INSTALLER, CFont::iconDownloadAlt());
  ui->tabWidget->setTabText(TAB_TERMINAL, CFont::iconEdit());
  ui->tabWidget->setTabText(TAB_PREFERENCES, CFont::iconCogs());
  ui->tabWidget->setTabEnabled(TAB_LABEL, false);

  ui->installTab->setLink(device);
  ui->terminalTab->setLink(device);
  ui->preferencesTab->setLink(device);

  connect(device, SIGNAL(linked(bool)), ui->installTab, SLOT(connected(bool)));
  connect(ui->installTab, SIGNAL(pauseTerminal(bool)), ui->terminalTab, SLOT(setPaused(bool)));
  connect(ui->installTab, SIGNAL(completed()), this, SLOT(installCompleted()));

  connect(ui->terminalTab, SIGNAL(resetTarget()), ui->installTab, SLOT(resetTarget()));
  connect(ui->terminalTab, SIGNAL(showPreferences()), this, SLOT(showPreferences()));

  ui->tabWidget->setCurrentIndex( settings.currentTab().toInt() );
  on_tabWidget_currentChanged(settings.currentTab().toInt());
}

CaosIsp::~CaosIsp()
{
  CLink * linkDevice;

  //wait for any pending operation on the device to complete
  linkDevice = ui->connectWidget->clink();
  if ( linkDevice->isConnected() == true ){
      linkDevice->disconnect();
    }
  delete ui;
}

void CaosIsp::showPreferences(void){
  ui->tabWidget->setCurrentIndex(TAB_PREFERENCES);
}

bool CaosIsp::installProgram(QString, QString){
  ui->tabWidget->setCurrentIndex(TAB_INSTALLER);
  return false;
}

void CaosIsp::installCompleted(void){
  Settings settings(Settings::global());
  if( settings.terminalShowAferInstall() == true ){
      ui->tabWidget->setCurrentIndex(TAB_TERMINAL);
    }
}


void CaosIsp::on_tabWidget_currentChanged(int index)
{
  Settings settings(Settings::global());
  switch(index){
    case TAB_INSTALLER:
      ui->tabWidget->setTabText(TAB_LABEL, "Installer");
      break;
    case TAB_TERMINAL:
      ui->tabWidget->setTabText(TAB_LABEL, "Terminal");
      break;
    case TAB_PREFERENCES:
      ui->tabWidget->setTabText(TAB_LABEL, "Preferences");
      break;
    }

  settings.setCurrentTab(QString::number(index));

}


void CaosIsp::operationComplete(QString msg, bool err){
  CNotify notify;
  if ( err == true ){
      notify.execError(msg);
    }  else if ( msg != "" ){
      if ( currentMessage != "" ){
          msg += ", " + currentMessage;
        }
      ui->statusBar->showMessage("      " + msg, 3000);
    }
}
