#include "CIsp.h"
#include "ui_CIsp.h"


#include <applib/Pin.hpp>
#include <applib/Uart.hpp>
#include <applib/Isp.hpp>
#include <applib/LpcIsp.hpp>

#include "CNotify.h"
#include "CFont.h"
#include "CSettings.h"

static CIsp * thisptr;

CIsp::CIsp(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CIsp)
{

    qDebug("Init ISP");
    ui->setupUi(this);

    terminalUart = 0;

    ui->resetButton->setObjectName("blueButton");
    ui->resetButton->setToolTip("Reset target");
    ui->resetButton->setText(CFont::iconRepeat());

    ui->programButton->setObjectName("blueButton");
    ui->programButton->setToolTip("Program target");
    ui->programButton->setText(CFont::iconDownloadAlt());

    ui->cancelButton->setObjectName("redButton");
    ui->cancelButton->setToolTip("Cancel operation");
    ui->cancelButton->setText(CFont::iconRemove());

    ui->startTerminalButton->setObjectName("greenButton");
    ui->startTerminalButton->setToolTip("Start Terminal");
    ui->startTerminalButton->setText(CFont::iconPlay());

    ui->pauseTerminalButton->setObjectName("redButton");
    ui->pauseTerminalButton->setToolTip("Pause Terminal");
    ui->pauseTerminalButton->setText(CFont::iconPause());

    ui->hideOptionsButton->setObjectName("defaultButton");
    ui->hideOptionsButton->setToolTip("Hide/show options");
    ui->hideOptionsButton->setText(CFont::iconList());

    ui->parityButton->setObjectName("defaultButton");
    ui->parityButton->setToolTip("Change Parity");
    ui->parityButton->setText("Even");
    ui->parityButton->setChecked(true);

    ui->stopBits->setObjectName("defaultButton");
    ui->stopBits->setToolTip("Change Stop bits");

    ui->uartLock->setObjectName("lock");
    ui->uartLock->setEnabled(true);
    ui->uartLock->setToolTip("Lock/unlock pin assignments");
    ui->uartSpinBox->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->uartSpinBox->setToolTip("Pin assignment for UART");

    ui->resetSpinBox->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->resetSpinBox->setToolTip("Pin number for reset");

    ui->ispreqSpinBox->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->ispreqSpinBox->setToolTip("Pin number for ISP Request");

    ui->uartSpinBox->setEnabled(false);
    ui->resetSpinBox->setEnabled(false);
    ui->ispreqSpinBox->setEnabled(false);

    ui->filename->setObjectName("unified");
    ui->filename->setBrowseVisible(true);
    ui->filename->setBrowseMode(QFileDialog::ExistingFile);

    ui->installKernel->setToolTip("Install kernel after install");

    ui->terminalBaudRate->setObjectName("unified");
    ui->terminalBaudRate->setBrowseVisible(false);
    ui->terminalBaudRate->lineEdit()->setAlignment(Qt::AlignHCenter);

    connect(ui->uartLock, SIGNAL(clicked(bool)), ui->uartSpinBox, SLOT(setEnabled(bool)));
    connect(ui->uartLock, SIGNAL(clicked(bool)), ui->resetSpinBox, SLOT(setEnabled(bool)));
    connect(ui->uartLock, SIGNAL(clicked(bool)), ui->ispreqSpinBox, SLOT(setEnabled(bool)));

    connected(false);
    setLink(ui->connect->clink());
    connect(ui->connect->clink(), SIGNAL(linked(bool)), this, SLOT(connected(bool)));
    qDebug("ISP Init Complete");

    //load preferences
    CSettings s(CSettings::userScope());

    ui->filename->lineEdit()->setText(s.getStringKey("ISP_FILENAME"));
    ui->terminalBaudRate->lineEdit()->setText( s.stringKey("ISP_TERMINAL_BAUDRATE"));
    switch(s.stringKey("ISP_TERMINAL_PARITY").toInt()){

    case UART_PARITY_EVEN:
        ui->parity->setChecked(true);
        ui->parityButton->setEnabled(true);
        ui->parityButton->setChecked(true);
        break;
    case UART_PARITY_ODD:
        ui->parity->setChecked(true);
        ui->parityButton->setEnabled(true);
        ui->parityButton->setChecked(false);
        break;
    default:
        /* no break */
    case UART_PARITY_NONE:
        ui->parity->setChecked(false);
        ui->parityButton->setEnabled(false);
        break;
    }

    on_stopBits_clicked( s.stringKey("ISP_TERMINAL_STOPBITS").toInt() == 1 );

    connect(&terminalTimer, SIGNAL(timeout()), this, SLOT(readTerminal()));
    connect(ui->parity, SIGNAL(clicked(bool)), ui->parityButton, SLOT(setEnabled(bool)));


    ui->installKernel->setChecked(s.boolKey("ISP_INSTALL_KERNEL"));

    thisptr = this;
    abort = false;
    optionsVisible = true;

}

CIsp::~CIsp()
{
    delete ui;
}

void CIsp::on_hideOptionsButton_clicked()
{

    optionsVisible = !optionsVisible;
    ui->filenameLabel->setVisible(optionsVisible);
    ui->filename->setVisible(optionsVisible);
    ui->uartLock->setVisible(optionsVisible);
    ui->uartLabel->setVisible(optionsVisible);
    ui->uartComboBox->setVisible(optionsVisible);
    ui->uartSpinBox->setVisible(optionsVisible);

    ui->resetLabel->setVisible(optionsVisible);
    ui->resetComboBox->setVisible(optionsVisible);
    ui->resetSpinBox->setVisible(optionsVisible);

    ui->ispreqLabel->setVisible(optionsVisible);
    ui->ispreqComboBox->setVisible(optionsVisible);
    ui->ispreqSpinBox->setVisible(optionsVisible);
    ui->connect->setVisible(optionsVisible);

    ui->optionsLabel->setVisible(optionsVisible);
    ui->installKernel->setVisible(optionsVisible);

}

void CIsp::resizeEvent(QResizeEvent * event){
    bool visible = true;
    if( (size().width() < 575) || (optionsVisible == false) ){
        visible = false;
    }
    ui->filenameLabel->setVisible(visible);
    ui->uartLabel->setVisible(visible);
    ui->resetLabel->setVisible(visible);
    ui->ispreqLabel->setVisible(visible);
    ui->optionsLabel->setVisible(visible);

    if( size().width() < 575){
        ui->terminalOptionsLabel->setVisible(false);
    } else {
        ui->terminalOptionsLabel->setVisible(true);
    }

    event = 0;
}

void CIsp::connected(bool v){

    ui->programButton->setEnabled(v);
    ui->startTerminalButton->setEnabled(v);
    ui->resetButton->setEnabled(v);

    ui->cancelButton->setEnabled(false);
    ui->pauseTerminalButton->setEnabled(false);

    if( v == true ){
        CSettings s(CSettings::userScope());

        populateComboBox("uart", ui->uartComboBox);
        populateComboBox("pio", ui->resetComboBox);
        populateComboBox("pio", ui->ispreqComboBox);

        //search for the saved settings
        ui->uartComboBox->setCurrentIndex(
              ui->uartComboBox->findText(s.stringKey("ISP_UART_PORT"))
              );
        ui->uartSpinBox->setValue( s.stringKey("ISP_UART_PINASSIGN").toInt());
        ui->resetComboBox->setCurrentIndex(
              ui->resetComboBox->findText(s.stringKey("ISP_RESET_PORT"))
              );

        ui->resetSpinBox->setValue( s.stringKey("ISP_RESET_PIN").toInt() );
        ui->ispreqComboBox->setCurrentIndex(
              ui->ispreqComboBox->findText(s.stringKey("ISP_ISPREQ_PORT"))
              );
        ui->ispreqSpinBox->setValue( s.stringKey("ISP_ISPREQ_PIN").toInt());
    } else {
        on_pauseTerminalButton_clicked();
    }
}


void CIsp::on_programButton_clicked()
{
    //start programming
    programTarget();
}

void CIsp::resetTarget(void){
  CNotify notify;

  Pin pio0(ui->resetComboBox->currentIndex(), ui->resetSpinBox->value());
  pio0.sethandle(link()->handle());

  if( pio0.init(Pin::OUTPUT) < 0 ){
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

  notify.updateStatus("Target reset complete");
}

void CIsp::populateComboBox(QString prefix, QComboBox * comboBox){
    QStringList devList;
    int i;
    comboBox->clear();
    devList = link()->dirList("/dev");
    for(i=0; i < devList.count(); i++){
        if( devList.at(i).startsWith(prefix) ){
            comboBox->addItem( devList.at(i) );
          }
      }
}

void CIsp::on_resetButton_clicked()
{
    resetTarget();
}

int CIsp::updateProgress(float prog){
    CNotify notify;

    notify.updateProgress(prog, 100, true);
    qApp->processEvents();

    //check for abort
    if( thisptr->abort == true ){
        return -1;
    }

  return 0;
}

void CIsp::programTarget(void)
{
  int ret;
  CNotify notify;
  QString filename;
  Isp * current;
  CSettings s(CSettings::userScope());

  ui->cancelButton->setEnabled(true);
  ui->programButton->setEnabled(false);
  ui->startTerminalButton->setEnabled(false);

  notify.updateProgress(0, 0, true);

  //save settings
  s.setKey("ISP_FILENAME", ui->filename->lineEdit()->text());
  s.setKey("ISP_UART_PORT", ui->uartComboBox->currentText());
  s.setKey("ISP_UART_PINASSIGN", QString::number(ui->uartSpinBox->value()));
  s.setKey("ISP_RESET_PORT", ui->resetComboBox->currentText());
  s.setKey("ISP_RESET_PIN", QString::number(ui->resetSpinBox->value()));
  s.setKey("ISP_ISPREQ_PORT", ui->ispreqComboBox->currentText());
  s.setKey("ISP_ISPREQ_PIN", QString::number(ui->ispreqSpinBox->value()));
  s.setKey("ISP_INSTALL_KERNEL", ui->installKernel->isChecked());


  filename = ui->filename->lineEdit()->text();

  Uart uart(ui->uartComboBox->currentIndex());
  Pin pio0(ui->resetComboBox->currentIndex(), ui->resetSpinBox->value());
  Pin pio1(ui->ispreqComboBox->currentIndex(), ui->ispreqSpinBox->value());
  uart.sethandle( link()->handle() );
  pio0.sethandle( link()->handle() );
  pio1.sethandle( link()->handle() );
  thisptr = this;

  LpcIsp lpc(&uart, &pio0, &pio1);

  ret = 0;
  this->abort = false;
  current = &lpc;

  //emit pauseTerminal(true);

  if( (ret = current->initphy(ui->uartSpinBox->value())) < 0 ){
      qDebug("INIT PHY ERROR");
      notify.updateStatus("Failed to init phy " + QString::number(ret));
    }

  qDebug("INIT PHY COMPLETE");
  if( ret == 0 ){
      if( current->program(ui->filename->lineEdit()->text().toLocal8Bit().constData(),
                           12000000,
                           "lpc1759",
                           &CIsp::updateProgress) < 0 ){
          qDebug("RET ERROR");
          notify.updateStatus("Failed to program target");
          ret = -1;
        }
    }

  qDebug("PROGRAM COMPLETE");
  if( ret == 0 ){
      if( current->exitphy() < 0 ){
          qDebug("EXIT PHY FAILED");
          notify.updateStatus("Failed to close UART");
          ret = -1;
        }
    }
  qDebug("EXIT PHY COMPLETE");

  thisptr = 0;

  CNotify::updateProgress(0, 0, false);

  ui->cancelButton->setEnabled(false);
  ui->programButton->setEnabled(true);
  ui->startTerminalButton->setEnabled(true);

  if( ret < 0 ){
      notify.execError("Failed to install");
    }

  if( ret == 0 ){
      //emit pauseTerminal(false);
    }

  if( ui->installKernel->isChecked() ){
      //emit a signal to install the kernel
      emit installKernel();
  }

}

void CIsp::on_cancelButton_clicked()
{
    abort = true;
}

void CIsp::on_parityButton_clicked(bool checked)
{
    if( checked == true ){
        ui->parityButton->setText("Even");
    } else {
        ui->parityButton->setText("Odd");
    }
}

void CIsp::on_startTerminalButton_clicked()
{
    uart_attr_t attr;
    int parity;
    int stopBits;
    //open the terminal
    if( link()->isConnected() == false ){
        CNotify::updateStatus("Host is not connected");
        return;
    }

    if( link()->isBootloader() == true ){
        CNotify::updateStatus("Host is a bootloader");
        return;
    }

    if( terminalUart != 0 ){
        delete terminalUart;
        terminalUart = 0;
    }

    if( terminalUart == 0 ){
        terminalUart = new Uart(ui->uartComboBox->currentIndex());
        if( terminalUart == 0 ){
            CNotify::updateStatus("Failed to alloc Uart");
            return;
        }

        CNotify::updateStatus("Set handle");
        terminalUart->sethandle(link()->handle());

        if( ui->parity->isChecked() ){
            if( ui->parityButton->isChecked() ){
                parity = UART_PARITY_EVEN;
            } else {
                parity = UART_PARITY_ODD;
            }
        } else {
            parity = UART_PARITY_NONE;
        }

        if( ui->stopBits->isChecked() ){
            stopBits = Uart::STOP1;
        } else {
            stopBits = Uart::STOP2;
        }


        if( terminalUart->open(Uart::NONBLOCK) < 0 ){
            CNotify::updateStatus("Failed to open UART");
            return;
        }

        attr.baudrate = ui->terminalBaudRate->lineEdit()->text().toInt();
        attr.stop = stopBits;
        attr.parity = (uart_parity_t)parity;
        attr.pin_assign = ui->uartSpinBox->value();
        attr.width = 8;
        attr.start = UART_ATTR_START_BITS_1;

        if( terminalUart->setattr(&attr) < 0 ){
            CNotify::updateStatus("Failed to start UART");
            delete terminalUart;
            terminalUart = 0;
        } else {

            if( terminalUart->getattr(&attr) <  0 ){
                CNotify::updateStatus("failed to get UART attr");
            } else {
                CNotify::updateStatus("Uart: " +
                                  QString::number( attr.baudrate ) +
                                      " Parity: " + QString::number(attr.parity) +
                                    " Stopbits: " + QString::number(attr.stop));
            }

            terminalTimer.setInterval(200);
            terminalTimer.start();

            ui->startTerminalButton->setEnabled(false);
            ui->pauseTerminalButton->setEnabled(true);
            ui->ispTerminal->opened(true);
            ui->terminalBaudRate->setEnabled(false);
            ui->stopBits->setEnabled(false);
            ui->parity->setEnabled(false);
            ui->parityButton->setEnabled(false);

            //save settings
            CSettings s(CSettings::userScope());
            s.setKey("ISP_TERMINAL_BAUDRATE", ui->terminalBaudRate->lineEdit()->text());
            s.setKey("ISP_TERMINAL_PARITY", QString::number(parity));
            s.setKey("ISP_TERMINAL_STOPBITS", QString::number(stopBits));
        }

    }


}

void CIsp::readTerminal(void){
    QString text;
    const int bufSize = 1024;
    char buffer[bufSize+1];
    int totalRead;
    int bytesRead;
    CNotify notify;

    if ( link()->isConnected() == false ){
        terminalTimer.stop();
        notify.execNotConnected();
        return;
      }

    if( terminalUart == 0 ){
        CNotify::updateStatus("Terminal not allocated");
        return;
      }


    totalRead = 0;
    //read the device and add text to the terminal
    do {
        memset(buffer, 0, bufSize+1);
        bytesRead = terminalUart->read(buffer, bufSize);
        if( bytesRead > 0 ){
            totalRead += bytesRead;
            buffer[bytesRead] = 0;
            text = QString(buffer);
            ui->ispTerminal->receiveText(text);
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
        //emit infoAvailable();
        terminalTimer.setInterval(1);
      } else {
        terminalTimer.setInterval(100);
      }
  }


void CIsp::on_stopBits_clicked(bool checked)
{
    ui->stopBits->setChecked(checked);

    if( checked == true ){
        ui->stopBits->setText("1");
    } else {
        ui->stopBits->setText("2");
    }
}

void CIsp::on_pauseTerminalButton_clicked()
{

    ui->startTerminalButton->setEnabled(true);
    ui->pauseTerminalButton->setEnabled(false);
    ui->ispTerminal->opened(false);

    ui->terminalBaudRate->setEnabled(true);
    ui->stopBits->setEnabled(true);
    ui->parity->setEnabled(true);
    ui->parityButton->setEnabled( ui->parity->isChecked() );

    if( terminalUart == 0 ){
        return;
    }


    if( link()->isConnected() == true ){
        terminalUart->close();
    }

    terminalTimer.stop();
    delete terminalUart;
    terminalUart = 0;
}


