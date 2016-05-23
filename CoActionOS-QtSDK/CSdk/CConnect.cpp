/* Copyright 2012; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "CConnect.h"
#include "ui_CConnect.h"
#include <QMessageBox>

#include "CFont.h"
#include "CNotify.h"

CConnect::CConnect(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::CConnect)
{
  QFont font;
  ui->setupUi(this);
  connect(&clink_, SIGNAL(linked(bool)), this, SLOT(connected(bool)));

  ui->frame->setObjectName("connectFrame");

  ui->connectButton->setToolTip("Connect to device");
  ui->disconnectButton->setToolTip("Disconnect from device");
  ui->refreshButton->setToolTip("Refresh the serial number list");
  ui->snComboBox->setToolTip("Device serial number list");

  errorSuppression = false;

  ui->connectButton->setObjectName("connectButton");
  ui->refreshButton->setObjectName("refreshButton");

  ui->refreshButton->setText(CFont::iconRefresh());
  ui->refreshButton->setObjectName("blueButton");
  ui->connectButton->setText(CFont::iconSignin());
  ui->connectButton->setObjectName("greenButton");
  ui->disconnectButton->setText(CFont::iconSignout());
  ui->disconnectButton->setObjectName("redButton");
  ui->disconnectButton->setEnabled(false);

  ui->connectStatusLabel->setVisible(true);
  ui->connectStatusLabel->setEnabled(false);
  ui->connectStatusLabel->setText("Disconnected");
  ui->connectStatusLabel->setObjectName("connectLabel");

  ui->bootloaderStatusLabel->setVisible(false);
  ui->bootloaderStatusLabel->setText("Bootloader");
  ui->bootloaderStatusLabel->setObjectName("blackLabel");

  this->refreshSerialno();

  periodic.setInterval(500);
  connect(&periodic, SIGNAL(timeout()), this, SLOT(timeout()));

}

CConnect::~CConnect()
{
  delete ui;
}

void CConnect::connected(bool status){
  ui->bootloaderStatusLabel->setVisible(false);
  ui->connectStatusLabel->setVisible(false);
  if ( status == true ){
      qDebug("Connected");
      if( ui->snComboBox->currentText() != "" ){
        CNotify::updateStatus("Connected to " + ui->snComboBox->currentText());
      } else {
          CNotify::updateStatus("Connected to anonymous");
      }
      if(clink_.isBootloader() == true ){
          ui->bootloaderStatusLabel->setVisible(true);
          CNotify::updateStatus("Connected device is bootloader");
        } else{
          ui->connectStatusLabel->setText("Connected");
          ui->connectStatusLabel->setEnabled(true);
          ui->connectStatusLabel->setVisible(true);
        }
      ui->connectButton->setEnabled(false);
      ui->disconnectButton->setEnabled(true);
      qDebug("start timer");
      periodic.start();
    } else {
      qDebug("Disconnected");
      if( ui->snComboBox->currentText() != "" ){
        CNotify::updateStatus("Disconnected from " + ui->snComboBox->currentText());
      } else {
          CNotify::updateStatus("Disconnected from anonymous");
      }
      ui->connectButton->setEnabled(true);
      ui->disconnectButton->setEnabled(false);
      ui->connectStatusLabel->setText("Disconnected");
      ui->connectStatusLabel->setEnabled(false);
      ui->connectStatusLabel->setVisible(true);
      periodic.stop();
    }

  ui->snComboBox->setEnabled(!status);
  ui->refreshButton->setEnabled(!status);
}

void CConnect::timeout(){
  CNotify notify;
  if( (clink_.isConnected() == false) && (ui->connectButton->isEnabled() == false) ){
      clink_.exit();
      notify.execError(ui->snComboBox->currentText() + " unexpectedly disconnected");
      CNotify::updateStatus(ui->snComboBox->currentText() + " unexpectedly disconnected");
    }
}

void CConnect::connectRequested(bool suppressError){
  errorSuppression = suppressError;
  on_connectButton_clicked();
  errorSuppression = false;
}

void CConnect::on_connectButton_clicked(){
  int err;
  QString buttonText;
  CNotify notify;
  buttonText = ui->connectButton->text();
  QStringList desc;
  QString serialNumber;

  qDebug("Connect Clicked");
  if (clink_.connected() ){
      //Already Connected
      ui->snComboBox->setEnabled(false);
      ui->disconnectButton->setEnabled(true);
      ui->connectButton->setEnabled(false);
    } else {
      //Need to connect
      ui->connectStatusLabel->setText("Connecting...");
      qApp->processEvents();

      desc = ui->snComboBox->currentText().split(":");
      if( desc.count() == 3 ){
          serialNumber = desc.at(2);
      } else {
          serialNumber = ui->snComboBox->currentText();
      }

      err = clink_.init(serialNumber.toStdString());

      qDebug("Connected? %d", err);
      if ( err != 0 ){
          //Error connecting
          ui->connectStatusLabel->setText("Disconnected");
          if ( errorSuppression == false ){
              notify.execError(QString(clink_.errorMessage().c_str()));
            }
          return;

        }

      //Compare the CoActionOS-Link Version to the core CoActionOS Version
    }

}

void CConnect::refresh(){
  if (clink_.connected() == true ){

    } else {
      if ( ui->connectButton->text() == "Disconnect" ){
          ui->connectButton->setText("Connect");
          ui->snComboBox->setEnabled(true);
        }
      refreshSerialno();
    }
}

void CConnect::refreshSerialno(){
  unsigned int i;
  vector<string> devices;
  devices = Link::listDevices(clink_.driver(), 64);
  ui->snComboBox->clear();
  for(i = 0; i < devices.size(); i++){
      ui->snComboBox->addItem( QString( devices[i].c_str() ) );
    }
  CNotify::updateStatus("Refreshed Serial Number List");
}


void CConnect::on_refreshButton_clicked(){
  refreshSerialno();
}

void CConnect::on_disconnectButton_clicked()
{
  CNotify notify;
  int err;
  if (clink_.connected() == true ){
      //Connected and need to disconnect
      ui->connectStatusLabel->setText("Disconnecting...");
      qApp->processEvents();
      err =clink_.exit();
      if ( err != 0 ){
          //Error disconnecting
          if ( errorSuppression == false ){
              notify.execError(errorMessage());
            }
          return;
        }
    } else {
      //already disconnected
      qDebug("Already Disconnected");
      ui->snComboBox->setEnabled(true);
      ui->connectButton->setEnabled(true);
      ui->disconnectButton->setEnabled(false);
    }
}

void CConnect::resizeEvent(QResizeEvent * event){
    QSize s = size();
    if( s.width() < 575 ){
        ui->snComboBox->hide();
        ui->refreshButton->hide();
    } else {
        ui->snComboBox->show();
        ui->refreshButton->show();
    }
    event = 0;
}


