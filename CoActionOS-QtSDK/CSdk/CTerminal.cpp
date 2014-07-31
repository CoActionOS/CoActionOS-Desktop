/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */


#include <QtWidgets/QScrollBar>
#include <QTextCursor>

#include "CNotify.h"
#include "CTerminal.h"
#include "ui_CTerminal.h"
#include "CFont.h"

#define MIN_SIZE (10*1024)

CTerminal::CTerminal(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CTerminal)
{
    ui->setupUi(this);
    connect(ui->sendLineEdit->lineEdit(), SIGNAL(returnPressed()), this, SLOT(on_sendButton_clicked()));
    ui->sendButton->setEnabled(false);
    ui->clearButton->setEnabled(true);
    ui->sendLineEdit->setObjectName("unified");

    maxSizeInt = MIN_SIZE; //4Kb minimum size

    ui->sendButton->setText(CFont::iconShareAlt());
    ui->sendButton->setObjectName("blueButton");
    ui->sendButton->setToolTip("Send text to device");
    ui->clearButton->setText(CFont::iconRemove());
    ui->clearButton->setObjectName("defaultButton");
    ui->clearButton->setToolTip("Clear terminal text");

    ui->textEdit->setTextInteractionFlags(Qt::TextSelectableByMouse);

}

CTerminal::~CTerminal(){
    delete ui;
}


void CTerminal::setMaxTextSize(int max){
    ui->textEdit->setMaxTextSize(max);
}

int CTerminal::maxTextSize(void){
    return ui->textEdit->maxTextSize();
}

bool CTerminal::isLogOpen(void){
  return logFile.isOpen();
}


void CTerminal::setLogFileName(QString name,
                               bool append, bool overwriteWarn){
  logFileNameVar = name;
  logAppendVar = append;
  logWarnOverwriteVar = overwriteWarn;
}

void CTerminal::opened(bool isOpen){
    CNotify notify;
    QFile::OpenMode mode;
    QString msg;
    int overwrite;

    ui->sendButton->setEnabled(isOpen);

    if( isOpen == true ){
        ui->logCheckBox->setEnabled(false);
        if( ui->logCheckBox->isChecked() == true ){
            overwrite = QDialog::Accepted;
            logFile.setFileName(logFileNameVar);
            if( logAppendVar == true ){
                mode = QIODevice::Append|QIODevice::WriteOnly;
              } else {
                mode = QIODevice::WriteOnly|QIODevice::Truncate;
                if( (logWarnOverwriteVar == true) && logFile.exists() ){
                    overwrite = notify.execPrompt("Overwrite Existing Log File?");
                  }
              }

            if( overwrite == QDialog::Accepted ){
                logFile.open(mode);
                if( logFile.isOpen() == false ){
                    ui->logCheckBox->setChecked(false);
                    CNotify::updateStatus("Log failed to open");
                  } else {
                    msg = "Log opened";
                    if( logAppendVar == true ){
                        msg = msg + " (append mode)";
                      }
                    CNotify::updateStatus(msg);
                  }
              } else {
                ui->logCheckBox->setChecked(false);
              }

          }
      } else {
        ui->logCheckBox->setEnabled(true);
        if( logFile.isOpen() == true ){
            CNotify::updateStatus("Log closed");
            logFile.close();
          }
    }

}

void CTerminal::receiveText(QString text){
    ui->textEdit->appendText(text, QColor(Qt::black));
    if( logFile.isOpen() == true ){
        logFile.write(text.toLocal8Bit().constData());
      }
}

/*! \details This method clears the terminal window.
 *
 */
void CTerminal::on_clearButton_clicked(){
    ui->textEdit->clear();
}


/*! \details This method sends text to the device when pressed.
 *
 */
void CTerminal::on_sendButton_clicked(){
    emit sendText(ui->sendLineEdit->lineEdit()->text());
    ui->sendLineEdit->lineEdit()->clear();
}

