#include "Preferences.h"
#include "ui_Preferences.h"

#include <CSdk/CFont.h>
#include "Settings.h"

Preferences::Preferences(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Preferences)
{
    Settings settings(Settings::global());
    ui->setupUi(this);
    ui->terminalDeviceRefreshButton->setText(CFont::iconRefresh());
    ui->terminalDeviceRefreshButton->setObjectName("blueButton");
    ui->terminalDeviceRefreshButton->setEnabled(false);

    ui->terminalLabel->setText(CFont::fontAwesome(CFont::iconTerminal()) + " Terminal");

    ui->terminalDevicePinConfigSpinBox->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->terminalDeviceLockCheckBox->setObjectName("lock");

    ui->terminalLogFile->setObjectName("unified");
    ui->terminalLogFile->setBrowseVisible(true);
    ui->terminalLogFile->setBrowseMode(QFileDialog::AnyFile, QFileDialog::AcceptSave);
    ui->terminalLogFile->lineEdit()->setText( settings.terminalLogFile() );
    ui->terminalDevicePinConfigSpinBox->setValue(
                settings.terminalDevicePinConfig().toInt()
                );

    ui->uartBaudrateComboBox->setCurrentIndex(
                ui->uartBaudrateComboBox->findText(
                    settings.uartBaudrate()
                    )
                );

    ui->uartParityComboBox->setCurrentIndex(
                ui->uartParityComboBox->findText(
                    settings.uartParity()
                    )
                );

    ui->uartStopComboBox->setCurrentIndex(
                ui->uartStopComboBox->findText(
                    settings.uartStopbits()
                    )
                );

    ui->terminalDeviceComboBox->addItem( settings.terminalDevice() );
    ui->terminalEnableAfterInstallCheckBox->setChecked( settings.terminalShowAferInstall() );
    connect(ui->terminalLogFile->lineEdit(), SIGNAL(editingFinished()), this, SLOT(terminalLogFile_editingFinished()));
    connect(ui->terminalLogFile, SIGNAL(fileDialogSelected()), this, SLOT(terminalLogFile_editingFinished()));
    connect(ui->terminalLogFile, SIGNAL(cleared()), this, SLOT(terminalLogFile_editingFinished()));

}

Preferences::~Preferences()
{
    delete ui;
}

void Preferences::setLink(CLink *d){
    CLinkPtr::setLink(d);
    connect(link(), SIGNAL(linked(bool)), this, SLOT(connected(bool)));
}

void Preferences::connected(bool value){
    ui->terminalDeviceRefreshButton->setEnabled(value);
}

void Preferences::on_terminalDeviceRefreshButton_clicked()
{
    QStringList devList;
    Settings settings(Settings::global());
    int i;
    ui->terminalDeviceComboBox->clear();
    devList = link()->dirList("/dev");
    for(i=0; i < devList.count(); i++){
        if( devList.at(i).startsWith("uart") ){
            ui->terminalDeviceComboBox->addItem( devList.at(i) );
        }
    }

    ui->terminalDeviceComboBox->setCurrentIndex(
                ui->terminalDeviceComboBox->findText(
                    settings.terminalDevice()
                    )
                );

}


void Preferences::on_terminalDeviceComboBox_currentIndexChanged(const QString &arg1)
{
    Settings settings(Settings::global());
    settings.setTerminalDevice(arg1);
}

void Preferences::on_terminalDevicePinConfigSpinBox_valueChanged(const QString &arg1)
{
    Settings settings(Settings::global());
    settings.setTerminalDevicePinConfig(arg1);
}

void Preferences::on_uartParityComboBox_currentIndexChanged(const QString &arg1)
{
    Settings settings(Settings::global());
    settings.setUartParity(arg1);
}

void Preferences::on_uartStopComboBox_currentIndexChanged(const QString &arg1)
{
    Settings settings(Settings::global());
    settings.setUartStopbits(arg1);
}

void Preferences::on_uartBaudrateComboBox_currentIndexChanged(const QString &arg1)
{
    Settings settings(Settings::global());
    settings.setUartBaudrate(arg1);
}

void Preferences::on_terminalDeviceLockCheckBox_clicked(bool checked)
{
    ui->terminalDeviceComboBox->setEnabled(!checked);
    ui->terminalDevicePinConfigSpinBox->setEnabled(!checked);
    ui->uartBaudrateComboBox->setEnabled(!checked);
    ui->uartStopComboBox->setEnabled(!checked);
    ui->uartParityComboBox->setEnabled(!checked);
}

void Preferences::on_terminalEnableAfterInstallCheckBox_clicked(bool checked)
{
    Settings settings(Settings::global());
    settings.setTerminalShowAfterInstall(checked);
}

void Preferences::terminalLogFile_editingFinished(void){
  Settings settings(Settings::global());
  settings.setTerminalLogFile(ui->terminalLogFile->lineEdit()->text());
}

