#include "Preferences.h"
#include "ui_Preferences.h"

#include <QSettings>
#include <QAction>
#include <QFileDialog>
#include <QFileInfo>

#include <CSdk/CFont.h>
#include <CSdk/CNotify.h>

#include "caoslink.h"


Preferences::Preferences(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::Preferences),
  settings(QSettings::UserScope, "CoActionOS Link")
{
  qDebug("Preferences Init");
  initialized = false;
  ui->setupUi(this);

  ui->savePreferencesButton->hide();
  ui->loadPreferencesButton->hide();
  ui->savePreferencesButton->setText(CFont::iconSave());
  ui->savePreferencesButton->setToolTip("Save Preferences");
  ui->savePreferencesButton->setObjectName("defaultButton");
  ui->loadPreferencesButton->setText(CFont::iconFolderOpen());
  ui->loadPreferencesButton->setToolTip("Load Saved Preferences");
  ui->loadPreferencesButton->setObjectName("defaultButton");

  ui->terminalLabel->setText(CFont::icomoon(CFont::iconTerminal()) + " Terminal");
  ui->terminalLabel->setEnabled(false);
  ui->terminalLabel->setObjectName("alertLabel");
  ui->terminalMaximumSizeLockCheckBox->setObjectName("lock");
  ui->terminalMaximumSizeLockCheckBox->setToolTip("Lock/unlock maximum size");
  ui->terminalMaximumSizeSpinBox->setAttribute(Qt::WA_MacShowFocusRect, 0);
  ui->terminalMaximumSizeSpinBox->setToolTip("Maximum memory size of terminal");
  ui->terminalAppendModeLabel->setToolTip("Append all data to one file (otherwise truncate)");
  ui->terminalAppendModeCheckBox->setToolTip("Append all data to one file (otherwise truncate)");
  ui->terminalWarnOverwriteLabel->setToolTip("Warn before truncating log");
  ui->terminalWarnOverwriteCheckBox->setToolTip("Warn before truncating log");
  ui->terminalLogFile->setObjectName("unified");
  ui->terminalLogFile->setBrowseVisible(true);
  ui->terminalLogFile->setBrowseMode(QFileDialog::AnyFile, QFileDialog::AcceptSave);

  ui->terminalLogFileStatusLabel->setObjectName("warningLabel");

  this->connected(false);
  loadPreferences();

  terminalLogFileLineEdit_editingFinished();

  connect(ui->terminalLogFile, SIGNAL(cleared()), this, SLOT(terminalLogFileLineEdit_editingFinished()));
  connect(ui->terminalLogFile, SIGNAL(fileDialogSelected()), this, SLOT(terminalLogFileLineEdit_editingFinished()));
  connect(ui->terminalLogFile->lineEdit(), SIGNAL(editingFinished()),
          this, SLOT(terminalLogFileLineEdit_editingFinished()));

  connect(ui->terminalMaximumSizeLockCheckBox, SIGNAL(clicked(bool)), ui->terminalMaximumSizeSpinBox, SLOT(setEnabled(bool)));

  lastMaxSize = ui->terminalMaximumSizeSpinBox->value();
  qDebug("Preferences Init Complete");
}

Preferences::~Preferences()
{
  delete ui;
}


void Preferences::loadPreferences(void){

  ui->terminalMaximumSizeSpinBox->setValue( settings.getStringKey("KEY_TERMINALMAXIMUMSIZE").toInt() );


  ui->terminalLogFile->lineEdit()->setText( settings.getStringKey("KEY_TERMINALLOGFILE") );
  ui->terminalAppendModeCheckBox->setChecked( settings.getBoolKey("KEY_TERMINALAPPENDMODE") );
  ui->terminalWarnOverwriteCheckBox->setChecked( settings.getBoolKey("KEY_TERMINALWARNOVERWRITE") );

}

void Preferences::setLink(CLink *d){
  CLinkPtr::setLink(d);
  connect(link(), SIGNAL(linked(bool)), this, SLOT(connected(bool)));
}

void Preferences::nohighlights(void){
  ui->terminalLabel->setEnabled(false);
}

void Preferences::startHiglightTimer(void){
  highlightTimer.setInterval(3000);
  highlightTimer.setSingleShot(true);
  connect(&highlightTimer, SIGNAL(timeout()), this, SLOT(nohighlights()), Qt::UniqueConnection);
  highlightTimer.start();
}

void Preferences::showTerminal(void){
  ui->scrollArea->ensureWidgetVisible(ui->terminalLabel);
  ui->scrollArea->ensureWidgetVisible(ui->terminalMaximumSizeLabel);
  ui->terminalLabel->setEnabled(true);
  startHiglightTimer();
}

void Preferences::connected(bool arg1){
  arg1 = false;
}


QString Preferences::terminalMaximumSize(void){
  CSettings s(QSettings::UserScope, Caoslink::appName());
  return s.getStringKey("KEY_TERMINALMAXIMUMSIZE");
}
QString Preferences::terminalLogFile(void){
  CSettings s(QSettings::UserScope, Caoslink::appName());
  return s.getStringKey("KEY_TERMINALLOGFILE");
}
bool Preferences::terminalAppendMode(void){
  CSettings s(QSettings::UserScope, Caoslink::appName());
  return s.getBoolKey("KEY_TERMINALAPPENDMODE");
}
bool Preferences::terminalWarnOverwrite(void){
  CSettings s(QSettings::UserScope, Caoslink::appName());
  return s.getBoolKey("KEY_TERMINALWARNOVERWRITE");
}
QString Preferences::currentTab(void){
  CSettings s(QSettings::UserScope, Caoslink::appName());
  return s.getStringKey("KEY_CURRENTTAB");
}
QString Preferences::applicationProject(void){
  CSettings s(QSettings::UserScope, Caoslink::appName());
  return s.getStringKey("KEY_APPLICATIONPROJECT");
}
QString Preferences::kernelProject(void){
  CSettings s(QSettings::UserScope, Caoslink::appName());
  return s.getStringKey("KEY_KERNELPROJECT");
}

QSize Preferences::windowSize(void){
    CSettings s(QSettings::UserScope, Caoslink::appName());
    QSize size;
    size.setHeight( s.getStringKey("KEY_MAINWINDOW_HEIGHT").toUInt() );
    size.setWidth( s.getStringKey("KEY_MAINWINDOW_WIDTH").toUInt() );
    return size;
}

QString Preferences::applicationConfiguration(void){
    CSettings s(QSettings::UserScope, Caoslink::appName());
    return s.getStringKey("KEY_APPLICATIONCONFIGURATION");
}


QString Preferences::kernelConfiguration(void){
    CSettings s(QSettings::UserScope, Caoslink::appName());
    return s.getStringKey("KEY_KERNELCONFIGURATION");
}


void Preferences::setCurrenTab(QString value){
  CSettings s(QSettings::UserScope, Caoslink::appName());
  s.setKey("KEY_CURRENTTAB", value);
}
void Preferences::setApplicationProject(QString value){
  CSettings s(QSettings::UserScope, Caoslink::appName());
  s.setKey("KEY_APPLICATIONPROJECT", value);
}
void Preferences::setkernelProject(QString value){
  CSettings s(QSettings::UserScope, Caoslink::appName());
  s.setKey("KEY_KERNELPROJECT", value);
}

void Preferences::setWindowSize(int w, int h){
    CSettings s(QSettings::UserScope, Caoslink::appName());
    s.setKey("KEY_MAINWINDOW_WIDTH", QString::number(w));
    s.setKey("KEY_MAINWINDOW_HEIGHT", QString::number(h));
}

void Preferences::setApplicationConfiguration(QString value){
    CSettings s(QSettings::UserScope, Caoslink::appName());
    s.setKey("KEY_APPLICATIONCONFIGURATION", value);
}

void Preferences::setkernelConfiguration(QString value){
    CSettings s(QSettings::UserScope, Caoslink::appName());
    s.setKey("KEY_KERNELCONFIGURATION", value);
}

void Preferences::terminalLogFileLineEdit_editingFinished(){
  QFileInfo info;
  if( ui->terminalLogFile->lineEdit()->text() == "" ){
      //ui->terminalLogFileLabel->setEnabled(false);
      ui->terminalLogFileStatusLabel->setText(CFont::fontAwesome(CFont::iconWarningSign()));
      ui->terminalLogFileStatusLabel->setEnabled(false);
      return;
    }

  info.setFile(ui->terminalLogFile->lineEdit()->text());
  if( info.isDir() == true ){
      ui->terminalLogFileStatusLabel->setText(CFont::fontAwesome(CFont::iconWarningSign()));
      ui->terminalLogFileStatusLabel->setEnabled(false);
      CNotify::updateStatus("Log file cannot be a directory");
      return;
    }

  if( info.dir().exists() == false ){
      ui->terminalLogFileStatusLabel->setText(CFont::fontAwesome(CFont::iconWarningSign()));
      ui->terminalLogFileStatusLabel->setEnabled(false);
      CNotify::updateStatus("Log file directory does not exist");
      return;
    }

  //ui->terminalLogFileLabel->setEnabled(true);
  ui->terminalLogFileStatusLabel->setText(CFont::fontAwesome(CFont::iconOkSign()));
  ui->terminalLogFileStatusLabel->setEnabled(true);


  settings.setKey("KEY_TERMINALLOGFILE",
         ui->terminalLogFile->lineEdit()->text());
}

void Preferences::on_terminalMaximumSizeLockCheckBox_toggled(bool checked)
{
  CNotify notify;

  if( checked == false ){
      lastMaxSize = ui->terminalMaximumSizeSpinBox->value();
    }


  if( (checked) && (lastMaxSize != ui->terminalMaximumSizeSpinBox->value()) ){
      settings.setKey("KEY_TERMINALMAXIMUMSIZE",
             QString::number(ui->terminalMaximumSizeSpinBox->value())
             );
      notify.execInfo("Change will be applied when program is restarted.");
    }
}

void Preferences::on_terminalAppendModeCheckBox_clicked(bool checked)
{
  settings.setKey("KEY_TERMINALAPPENDMODE", checked);
  ui->terminalWarnOverwriteCheckBox->setEnabled(!checked);
  if( checked ){
      ui->terminalWarnOverwriteCheckBox->setChecked(false);
    } else {
      ui->terminalWarnOverwriteCheckBox->setChecked( terminalWarnOverwrite() );
    }
}

void Preferences::on_terminalWarnOverwriteCheckBox_clicked(bool checked)
{
  settings.setKey("KEY_TERMINALWARNOVERWRITE", checked);
}

void Preferences::on_savePreferencesButton_clicked()
{
  QFileDialog dialog;
  connect(&dialog, SIGNAL(fileSelected(QString)), this, SLOT(savePreferencesButton_complete(QString)));
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  dialog.exec();
}

void Preferences::savePreferencesButton_complete(QString name){
  qDebug("Save file %s", name.toLocal8Bit().constData());
}

