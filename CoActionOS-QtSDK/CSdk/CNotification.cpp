#include "CNotification.h"
#include "ui_CNotification.h"

#include <QApplication>

#include "CFont.h"

CNotification::CNotification(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CNotification)
{
    ui->setupUi(this);

    this->setObjectName("bordered");
    ui->okButton->setObjectName("greenButton");
    ui->okButton->setText(CFont::iconOk());
    ui->cancelButton->setObjectName("redButton");
    ui->cancelButton->setText(CFont::iconRemove());
    ui->dismissButton->setObjectName("defaultButton");
    ui->dismissButton->setText(CFont::iconRemove());
    ui->infoLabel->setText(CFont::icomoon(CFont::iconInfoSign()) +
                           " Notice");
    ui->infoLabel->setObjectName("acceptInfoLabel");
    ui->warningLabel->setText(CFont::icomoon(CFont::iconWarningSign()) +
                              " Warning");
    ui->warningLabel->setObjectName("alertWarningLabel");
    ui->errorLabel->setText(CFont::icomoon(CFont::iconExclamationSign()) +
                            " Error");
    ui->errorLabel->setObjectName("alertErrorLabel");
    ui->questionLabel->setText(CFont::icomoon(CFont::iconQuestionSign()) +
                               " Question");
    ui->questionLabel->setObjectName("acceptInfoLabel");

    ui->cancelButton->hide();
    ui->okButton->hide();

    hideAll();
}


CNotification::~CNotification()
{
    delete ui;
}

void CNotification::setWarning(const QString & msg){
    hideAll();
    ui->dismissButton->show();
    ui->warningLabel->show();
    ui->messageLabel->setText(msg);
    ui->messageLabel->adjustSize();
    QApplication::alert(this, 0);
    emit activated(SEVERITY_MED);
}

void CNotification::setError(const QString & msg){
    hideAll();
    ui->dismissButton->show();
    ui->errorLabel->show();
    ui->messageLabel->setText(msg);
    ui->messageLabel->adjustSize();
    QApplication::alert(this, 0);
    emit activated(SEVERITY_HIGH);
}

void CNotification::setQuestion(const QString & msg){
    hideAll();
    ui->cancelButton->show();
    ui->okButton->show();
    ui->questionLabel->show();
    ui->messageLabel->setText(msg);
    ui->messageLabel->adjustSize();
    QApplication::alert(this, 0);
    emit activated(SEVERITY_MED);
}

void CNotification::setInfo(const QString & msg){
    hideAll();
    ui->dismissButton->show();
    ui->infoLabel->show();
    ui->messageLabel->setText(msg);
    ui->messageLabel->adjustSize();
    QApplication::alert(this, 0);
    emit activated(SEVERITY_LOW);

}

void CNotification::hideAll(void){
    ui->infoLabel->setVisible(false);
    ui->warningLabel->setVisible(false);
    ui->errorLabel->setVisible(false);
    ui->questionLabel->setVisible(false);
    ui->cancelButton->hide();
    ui->okButton->hide();
    ui->dismissButton->hide();
}


void CNotification::on_okButton_clicked()
{
    emit dismissed(1);
}

void CNotification::on_cancelButton_clicked()
{
    emit dismissed(0);
}

void CNotification::on_dismissButton_clicked()
{
    emit dismissed(0);
}
