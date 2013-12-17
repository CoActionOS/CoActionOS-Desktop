#include "CInstaller.h"
#include "ui_CInstaller.h"

#include <QFile>

#include "CFont.h"
#include "CNotify.h"
#include "CSettings.h"

CInstaller::CInstaller(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::CInstaller)
{
  ui->setupUi(this);
  updateConfiguration = false;
  ui->configurationComboBox->setToolTip("Build configuration");
  ui->projectNameComboBox->setToolTip("Choose project in workspace");

  ui->refreshWorkspaceButton->setObjectName("defaultButton");
  ui->refreshWorkspaceButton->setToolTip("Refresh workspace");
  ui->refreshWorkspaceButton->setText(CFont::iconRefresh());

  ui->addProjectButton->setObjectName("greenButton");
  ui->addProjectButton->setToolTip("Add project to workspace");
  ui->addProjectButton->setText(CFont::iconPlus());

  ui->addConfigurationButton->setObjectName("greenButton");
  ui->addConfigurationButton->setToolTip("Add configuration to project");
  ui->addConfigurationButton->setText(CFont::iconPlus());

  ui->clearConfigurationsButton->setObjectName("defaultButton");
  ui->clearConfigurationsButton->setToolTip("Clear all configurations");
  ui->clearConfigurationsButton->setText(CFont::iconRemove());

  ui->projectNameComboBox->model()->setObjectName("popup");

  ui->workspaceLine->setObjectName("unified");
  ui->workspaceLine->setBrowseVisible(true);
  ui->workspaceLine->setBrowseMode(QFileDialog::DirectoryOnly, QFileDialog::AcceptOpen);

  connect(ui->workspaceLine->lineEdit(), SIGNAL(editingFinished()), this, SLOT(workspaceUpdated()));
  connect(ui->workspaceLine, SIGNAL(fileDialogSelected()), this, SLOT(workspaceUpdated()));
  connect(ui->workspaceLine, SIGNAL(cleared()), this, SLOT(workspaceUpdated()));

}

/*
void Preferences::workspaceEditingFinished(QLabel * label, QLineEdit * lineEdit){

}
*/

CInstaller::~CInstaller()
{
  delete ui;
}


void CInstaller::workspaceUpdated(){
  CSettings s(QSettings::UserScope, "CoActionOS-QtSDK");
  QFileInfo info;
  QString text;
  CNotify notify;
  QDir dir(ui->workspaceLine->lineEdit()->text());

  //change any backslashes to slashes
  text = ui->workspaceLine->lineEdit()->text().replace('\\', '/');
  info.setFile(text);
  if( info.exists() == false ){
      ui->projectNameComboBox->clear();
      return;
    }

  if( info.isDir() == false ){
      text = info.dir().path();
    }
  ui->workspaceLine->lineEdit()->setText( text.replace('\\', '/') );


  if( dir.exists() == false ){
      notify.execError("Workspace not found");
      return;
    }

  s.setKey(QString("KEY_" + settings), ui->workspaceLine->lineEdit()->text());

  updateProjectList();
}

void CInstaller::setWorkspace(QString workspace){
  ui->workspaceLine->lineEdit()->setText(workspace);
  workspaceUpdated();
}

QString CInstaller::workspace(void){
  return ui->workspaceLine->lineEdit()->text();
}

QString CInstaller::project(){
  return ui->projectNameComboBox->currentText();
}

QString CInstaller::configuration(){
  return ui->configurationComboBox->currentText();
}


void CInstaller::on_addConfigurationButton_clicked()
{
  QFileDialog dialog;
  QStringList newConfigs;
  dialog.setAcceptMode(QFileDialog::AcceptOpen);
  dialog.setFileMode(QFileDialog::DirectoryOnly);
  dialog.setDirectory(projectPath());
  dialog.exec();

  ui->configurationComboBox->clear();
  ui->configurationComboBox->addItems(newConfigs);
  updateConfiguration = false;
  addConfigurations(dialog.selectedFiles());
  updateConfiguration = true;

}

void CInstaller::addConfigurations(QStringList list){
  int i, j;
  bool duplicate;
  QString config;
  QStringList current;
  CSettings s(projectPath() + "/" + settings, false);
  s.addKey("KEY_configurations");
  if( s.exists() ){
      current = s.getStringListKey("KEY_configurations");

      for(i=0; i < list.count(); i++){
          QFileInfo info(list.at(i));
          config = info.fileName();
          duplicate = false;
          for(j=0; j < current.count(); j++){
              if( current.at(j) == config ){
                  duplicate = true;
                  break;
                }
            }

          if(!duplicate){
              current.append(config);
            }
        }
    }

  s.setKey("KEY_configurations", current);
  ui->configurationComboBox->addItems(current);
}


void CInstaller::on_refreshWorkspaceButton_clicked()
{
  workspaceUpdated();
}

void CInstaller::on_addProjectButton_clicked()
{
  QFileDialog dialog;
  QStringList list;
  CNotify notify;
  dialog.setAcceptMode(QFileDialog::AcceptOpen);
  dialog.setFileMode(QFileDialog::DirectoryOnly);
  dialog.setDirectory(workspace());
  dialog.exec();

  list = dialog.selectedFiles();
  if( list.count() > 0 ){
      QFileInfo info(list.at(0));
      if( info.path() != workspace() ){
          notify.execError("Project is not is current workspace");
          return;
        }
      emit addProject( info.fileName() );
      updateProjectList();
    }
}


void CInstaller::updateProjectList(void){
  QDir dir(workspace());
  int i;
  ui->projectNameComboBox->clear();
  for(i=0; i < dir.entryList().count(); i++){
      QFileInfo info( workspace() + "/" + dir.entryList().at(i) + "/" + settings );
      if( (info.exists()) && (info.dir().dirName() != ".") && (info.dir().dirName() != "..") ){
          ui->projectNameComboBox->addItem( info.dir().dirName() );
        }
    }
}

void CInstaller::on_clearConfigurationsButton_clicked()
{
  CSettings s(projectPath() + "/" + settings, false);
  ui->configurationComboBox->clear();
  if( s.exists() ){
      s.setKey("KEY_configurations", QStringList());
    }
}

void CInstaller::on_projectNameComboBox_currentIndexChanged(const QString &arg1)
{
  CSettings s(projectPath() + "/" + settings, false);
  QString current;
  int currentIndex;
  updateConfiguration = false;
  ui->configurationComboBox->clear();
  addConfigurations(QStringList());
  current = s.getStringKey("KEY_configuration");

  currentIndex = ui->configurationComboBox->findText(current);
  if( currentIndex >= 0 ){
    ui->configurationComboBox->setCurrentIndex(currentIndex);
    }

  updateConfiguration = true;
  emit projectSelected(arg1);
}

void CInstaller::on_configurationComboBox_currentIndexChanged(const QString &arg1)
{
  if( updateConfiguration ){
      CSettings s(projectPath() + "/" + settings, false);
      s.setKey("KEY_configuration", arg1);
    }
}
