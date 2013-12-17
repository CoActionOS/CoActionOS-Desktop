#include "IspWindow.h"
#include "ui_IspWindow.h"

IspWindow::IspWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::IspWindow)
{
    ui->setupUi(this);

    connect(ui->lpc, SIGNAL(closeTerminal()), this, SIGNAL(closeTerminal()));
    connect(ui->stm32, SIGNAL(closeTerminal()), this, SIGNAL(closeTerminal()));
    connect(ui->lpc, SIGNAL(openTerminal()), this, SIGNAL(openTerminal()));
    connect(ui->stm32, SIGNAL(openTerminal()), this, SIGNAL(openTerminal()));
}

IspWindow::~IspWindow()
{
    delete ui;
}
