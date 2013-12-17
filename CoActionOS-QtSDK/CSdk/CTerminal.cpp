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
    maxSizeInt = max;
    if( maxSizeInt < MIN_SIZE ){
        maxSizeInt = MIN_SIZE;
    }
}

int CTerminal::maxTextSize(void){
    return maxSizeInt;
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


/*! \details This method adds text to the terminal window.  It
 * is designed to auto scroll to the bottom when new text is added unless
 * the user has scroll up.
 *
 */
void CTerminal::addText(QString text, const QColor & textColor){
    int currentScroll;
    int maximumScroll;
    QTextCursor currentCursor;
    QTextCursor insertCursor;

    currentScroll = ui->textEdit->verticalScrollBar()->value();
    maximumScroll = ui->textEdit->verticalScrollBar()->maximum();
    currentCursor = ui->textEdit->textCursor();
    insertCursor = currentCursor;

    //Put the write text in a different color
    insertCursor.movePosition(QTextCursor::End);
    insertCursor.clearSelection();
    ui->textEdit->setTextCursor(insertCursor);
    ui->textEdit->setTextColor(textColor);
    ui->textEdit->insertPlainText(text);

    if ( ui->textEdit->toPlainText().size() > maxSizeInt ){
        ui->textEdit->setPlainText( ui->textEdit->toPlainText().right(maxSizeInt/2) );
        ui->textEdit->textCursor().movePosition(QTextCursor::End);
        ui->textEdit->verticalScrollBar()->setValue( ui->textEdit->verticalScrollBar()->maximum() );
    }

    ui->textEdit->setTextCursor(currentCursor);

    if ( currentScroll == maximumScroll ){
        ui->textEdit->verticalScrollBar()->setValue( ui->textEdit->verticalScrollBar()->maximum() );
    } else {
        ui->textEdit->verticalScrollBar()->setValue(currentScroll);
    }

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
    addText(text, QColor(Qt::black));
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

