#include "CIconLabel.h"
#include "ui_CIconLabel.h"

CIconLabel::CIconLabel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CIconLabel)
{
    ui->setupUi(this);
    ui->frame->setObjectName("unifiedLabelFrame");
    ui->iconLabel->setObjectName("unifiedLabelIcon");
    ui->textLabel->setObjectName("unifiedLabelText");
}

CIconLabel::~CIconLabel()
{
    delete ui;
}

QLabel * CIconLabel::icon(){
    return ui->iconLabel;
}

QLabel * CIconLabel::label(){
    return ui->textLabel;
}

void CIconLabel::setText(QString icon, QString label){
    ui->iconLabel->setText(icon);
    ui->textLabel->setText(label);
}
