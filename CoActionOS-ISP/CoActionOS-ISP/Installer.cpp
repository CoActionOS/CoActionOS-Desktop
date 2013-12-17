#include "Installer.h"
#include "ui_Installer.h"

#include <CSdk/CNotify.h>
#include <CSdk/CFont.h>

#include "Settings.h"

static Installer * thisptr;

Installer::Installer(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::Installer)
{
  ui->setupUi(this);

  ui->serialPinConfigSpinBox->setAttribute(Qt::WA_MacShowFocusRect, 0);
  ui->pio0PinSpinBox->setAttribute(Qt::WA_MacShowFocusRect, 0);
  ui->pio1PinSpinBox->setAttribute(Qt::WA_MacShowFocusRect, 0);

  ui->serialPinConfigLockCheckBox->setObjectName("lock");

  ui->file->setObjectName("unified");
  ui->file->setBrowseVisible(true);
  ui->file->setBrowseMode(QFileDialog::AnyFile);

  ui->progressBar->hide();
  ui->goButton->setText(CFont::iconPlay());
  ui->goButton->setObjectName("greenButton");
  ui->goButton->setEnabled(false);
  ui->goButton->setToolTip("Start Programming");
  ui->cancelButton->setText(CFont::iconRemove());
  ui->cancelButton->setObjectName("redButton");
  ui->cancelButton->setEnabled(false);
  ui->cancelButton->setToolTip("Cancel Programming");

  on_lpcCheckBox_clicked(true);
}

Installer::~Installer()
{
  delete ui;
}


void Installer::resetTarget(void){
  CNotify notify;

  Pin pio0(ui->pio0PortComboBox->currentIndex(), ui->pio0PinSpinBox->value());
  pio0.sethandle(link()->handle());

  if( pio0.init(PIO_MODE_OUTPUT) < 0 ){
      notify.execError("Failed to init reset pin");
      return;
    }

  if( pio0.clr() < 0 ){
      notify.execError("Failed to clear reset pin");
      pio0.close();
      return;
    }

  if( pio0.set() < 0 ){
      notify.execError("Failed to clear reset pin");
      pio0.close();
      return;
    }

  if( pio0.setinput() < 0 ){
      notify.execError("Failed to clear reset pin");
      pio0.close();
      return;
    }

  pio0.close();
}

void Installer::setLink(CLink *d){
  CLinkPtr::setLink(d);
  connect(d, SIGNAL(linked(bool)), this, SLOT(connected(bool)));
}

void Installer::connected(bool value){
  ui->goButton->setEnabled(value);
  //ui->refreshButton->setEnabled(value);

  if( value == true ){
      QStringList devList;
      Settings settings(Settings::global());
      int i;
      ui->serialPortComboBox->clear();
      ui->pio0PortComboBox->clear();
      ui->pio1PortComboBox->clear();
      devList = link()->dirList("/dev");
      for(i=0; i < devList.count(); i++){
          if( devList.at(i).startsWith("uart") ){
              ui->serialPortComboBox->addItem( devList.at(i) );
            }
          if( devList.at(i).startsWith("pio") ){
              ui->pio0PortComboBox->addItem( devList.at(i) );
              ui->pio1PortComboBox->addItem( devList.at(i) );
            }
        }

      ui->file->lineEdit()->setText(settings.ispFilename());
      ui->pio0PortComboBox->setCurrentIndex(
            ui->pio0PortComboBox->findText(settings.ispPioPort0())
            );
      ui->pio1PortComboBox->setCurrentIndex(
            ui->pio1PortComboBox->findText(settings.ispPioPort1())
            );
      ui->pio0PinSpinBox->setValue( settings.ispPioPin0().toInt() );
      ui->pio1PinSpinBox->setValue( settings.ispPioPin1().toInt() );

    }
}

void Installer::on_lpcCheckBox_clicked(bool checked)
{
  char device[ Isp::name_maxsize() ];
  char pio0[ Isp::name_maxsize() ];
  char pio1[ Isp::name_maxsize() ];
  bool mode = checked;
  LpcIsp lpc(0, 0, 0);
  ui->stm32CheckBox->setChecked(!mode);

  lpc.copy_names(device, pio0, pio1);

  ui->pio0Label->setText( QString(pio0) + ":" );
  ui->pio1Label->setText( QString(pio1) + ":" );
}

void Installer::on_stm32CheckBox_clicked(bool checked)
{
  bool mode = !checked;
  ui->lpcCheckBox->setChecked(mode);
}

bool Installer::installProgram(QString path, QString arch){


  ui->file->lineEdit()->setText(path);
  ui->lpcCheckBox->setChecked(false);
  ui->stm32CheckBox->setChecked(false);

  thisptr = this;

  qDebug("install arch %s", arch.toLocal8Bit().constData());
  if( arch == "LPC" ){
      ui->lpcCheckBox->setChecked(true);
    } else if( arch == "STM32" ){
      ui->stm32CheckBox->setChecked(true);
    }

  on_goButton_clicked();
  return true;
}

void Installer::on_goButton_clicked()
{
  int ret;
  QString filename;
  Isp * current;
  Settings settings(Settings::global());
  ui->cancelButton->setEnabled(true);
  ui->goButton->setEnabled(false);
  ui->progressBar->show();
  ui->progressBar->setMaximum(0);

  //save settings
  settings.setIspPioPort0(ui->pio0PortComboBox->currentText());
  settings.setIspPioPin0(ui->pio0PinSpinBox->text());
  settings.setIspPioPort1(ui->pio1PortComboBox->currentText());
  settings.setIspPioPin1(ui->pio1PinSpinBox->text());
  settings.setIspFilename(ui->file->lineEdit()->text());

  filename = ui->file->lineEdit()->text();

  Uart uart(ui->serialPortComboBox->currentIndex());
  Pin pio0(ui->pio0PortComboBox->currentIndex(), ui->pio0PinSpinBox->value());
  Pin pio1(ui->pio1PortComboBox->currentIndex(), ui->pio1PinSpinBox->value());
  uart.sethandle( link()->handle() );
  pio0.sethandle( link()->handle() );
  pio1.sethandle( link()->handle() );
  thisptr = this;

  LpcIsp lpc(&uart, &pio0, &pio1);

  ret = 0;
  if( ui->lpcCheckBox->isChecked() == true ){
      this->abort = false;
      current = &lpc;
    } else if ( ui->stm32CheckBox->isChecked() == true ){
      //ret = ui->stm32IspWidget->installProgram(filename, ui->progressBar);
    }

  emit pauseTerminal(true);
  if( current->initphy(ui->serialPinConfigSpinBox->value()) < 0 ){
      qDebug("INIT PHY ERROR");
      ret = -1;
    }


  qDebug("INIT PHY COMPLETE");
  if( ret == 0 ){
      if( current->program(ui->file->lineEdit()->text().toLocal8Bit().constData(), 12000000, "lpc1759", &Installer::updateProgress) < 0 ){
          qDebug("RET ERROR");
          ret = -1;
        }
    }

  qDebug("PROGRAM COMPLETE");
  if( ret == 0 ){
      if( current->exitphy() < 0 ){
          qDebug("EXTI PHY FAILED");
          ret = -1;
        }
    }
  qDebug("EXIT PHY COMPLETE");

  thisptr = 0;

  ui->progressBar->hide();
  ui->progressBar->setMaximum(0);

  ui->cancelButton->setEnabled(false);
  ui->goButton->setEnabled(true);

  if( ret < 0 ){
      CNotify::updateStatus("Failed to Install");
    }

  if( ret == 0 ){
      emit pauseTerminal(false);
    }

}

void Installer::on_cancelButton_clicked()
{
  this->abort = true;
}

void Installer::on_serialPinConfigLockCheckBox_clicked(bool checked)
{
  ui->serialPortComboBox->setEnabled(!checked);
  ui->serialPinConfigSpinBox->setEnabled(!checked);
  ui->pio0PinSpinBox->setEnabled(!checked);
  ui->pio0PortComboBox->setEnabled(!checked);
  ui->pio1PinSpinBox->setEnabled(!checked);
  ui->pio1PortComboBox->setEnabled(!checked);

}

int Installer::updateProgress(float prog){
  if ( thisptr != NULL ){
      thisptr->ui->progressBar->setMaximum(100);
      thisptr->ui->progressBar->setValue( prog );
      qApp->processEvents();
      if ( thisptr->abort == true ){
          return -1;
        } else {
          return 0;
        }
    }

  return 0;
}

