#include "IspTerminal.h"
#include "ui_IspTerminal.h"

#include <CSdk/CFont.h>
#include <CSdk/CNotify.h>
#include <Applib/Uart.hpp>

#include "Settings.h"

IspTerminal::IspTerminal(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::IspTerminal)
{
  ui->setupUi(this);

  ui->openButton->setText(CFont::iconPlay());
  ui->openButton->setObjectName("greenButton");
  ui->openButton->setEnabled(false);
  ui->openButton->setToolTip("Open Terminal");
  ui->closeButton->setText(CFont::iconPause());
  ui->closeButton->setObjectName("redButton");
  ui->closeButton->setEnabled(false);
  ui->closeButton->setToolTip("Close Terminal");

  ui->resetButton->setText(CFont::iconRepeat());
  ui->resetButton->setObjectName("blueButton");
  ui->resetButton->setEnabled(false);
  ui->resetButton->setToolTip("Reset Target Device");

  connect(ui->terminalWidget, SIGNAL(sendText(QString)), this, SLOT(writeDevice(QString)));
  connect(&readTimer, SIGNAL(timeout()), this, SLOT(readDevice()));

  ui->terminalWidget->setMaxTextSize(1024*500); //500kB max

  uart = 0;
}


IspTerminal::~IspTerminal()
{
  if( uart != 0 ){
      delete uart;
      uart = 0;
    }
  delete ui;
}

void IspTerminal::setLink(CLink *d){
  CLinkPtr::setLink(d);
  connect(link(), SIGNAL(linked(bool)), this, SLOT(connected(bool)));
}

void IspTerminal::connected(bool arg1){
  CNotify notify;

  if( link()->isBootloader() ){
      if( arg1 ){ notify.execError("Connected to Bootloader"); }
      return;
    }

  if( arg1 == false ){
      on_closeButton_clicked();
    }

  ui->openButton->setEnabled(arg1);
  ui->resetButton->setEnabled(arg1);
}


void IspTerminal::on_openButton_clicked()
{
  Settings settings(Settings::global());
  QString tmp;
  CNotify notify;
  int ret;
  uart_attr_t attr;
  int port;

  if ( link()->isConnected() == false ){
      qDebug("Tried to openTerminal()");
      return;
    }

  qDebug("CaosTerminal: Check for Bootloader or Normal mode");
  if ( link()->isBootloader() > 0 ){
      return;
    }

  if( uart != 0 ){
      uart->close();
      delete uart;
    }

  port = 0;
  uart = new Uart(port);
  uart->sethandle( link()->handle() );

  tmp = settings.terminalDevice();
  if( tmp == "" ){
      ret = notify.execPrompt("Terminal device not set.  Change Preferences?");
      if( ret == 0 ){
          CNotify::updateStatus("Terminal Open Aborted");
        } else {
          emit showPreferences();
        }
    }

  //set up the UART using ioctl
  attr.baudrate = settings.uartBaudrate().toInt();
  attr.stop = settings.uartStopbits().toInt();
  attr.parity = (uart_parity_t)settings.uartParity().toInt();
  attr.pin_assign = settings.terminalDevicePinConfig().toInt();
  attr.width = 8;
  attr.start = UART_ATTR_START_BITS_1;
  if( uart->open(Uart::NONBLOCK) < 0 ){
      qDebug("Failed to open UART");
      notify.execError("Failed to open UART");
      delete uart;
      return;
    }

  if( uart->setattr(&attr) < 0 ){
      qDebug("Failed to set UART attr");
      notify.execError("Failed to configure UART");
    }

  ui->terminalWidget->setLogFileName(settings.terminalLogFile());
  ui->terminalWidget->opened(true);

  ui->closeButton->setEnabled(true);
  ui->openButton->setEnabled(false);
  emit opened(true);

  readTimer.setInterval(200);
  readTimer.start();
}

void IspTerminal::on_closeButton_clicked()
{
  readTimer.stop();
  if( uart != 0 ){
      if( link()->isConnected() ){
          if( uart->close() < 0 ){
              qDebug("Failed to close input device");
            }
        } else {
          qDebug("Device is not connected");
        }
      delete uart;
      uart = 0;
    }
  ui->closeButton->setEnabled(false);
  ui->openButton->setEnabled(true);
  ui->terminalWidget->opened(false);
  emit opened(false);
}

void IspTerminal::setPaused(bool arg1){
  if( arg1 == true ){
      on_closeButton_clicked();
    } else {
      on_openButton_clicked();
    }
}


void IspTerminal::readDevice(void){
  QString text;
  const int bufSize = 1024;
  char buffer[bufSize+1];
  int totalRead;
  int bytesRead;
  CNotify notify;

  if ( link()->isConnected() == false ){
      readTimer.stop();
      notify.execNotConnected();
      return;
    }

  if( uart == 0 ){
      return;
    }


  totalRead = 0;
  //read the device and add text to the terminal
  do {
      memset(buffer, 0, bufSize+1);
      bytesRead = uart->read(buffer, bufSize);
      if( bytesRead > 0 ){
          totalRead += bytesRead;
          buffer[bytesRead] = 0;
          text = QString(buffer);
          ui->terminalWidget->receiveText(text);
          if( totalRead > 4096 ){
              break;
            }
        }

      if( bytesRead < 0 ){
          qDebug("Failed to read");
          qDebug("Error %d %d\n", bytesRead, link_errno);
        }

    } while( bytesRead > 0);

  if( totalRead > 0 ){
      emit infoAvailable();
      readTimer.setInterval(1);
    } else {
      readTimer.setInterval(100);
    }
}

void IspTerminal::writeDevice(QString sendString){
  int err;
  CNotify notify;
  if ( link()->isConnected() == false ){
      notify.execNotConnected();
      return;
    }


  if ( link()->isConnected() && (uart != 0) ){

      sendString += "\n";

      err = uart->write(sendString.toLocal8Bit().constData(),
                        sendString.size());

      if ( err < 0 ){
          notify.execError("Failed to Send Data: " + errorMessage());
          return;
        }

    } else {
      qDebug("Stop not connected");
      readTimer.stop();
      notify.execNotConnected();
    }
}

void IspTerminal::on_resetButton_clicked()
{
  qDebug("Emit Reset Target");
  emit resetTarget();
}
