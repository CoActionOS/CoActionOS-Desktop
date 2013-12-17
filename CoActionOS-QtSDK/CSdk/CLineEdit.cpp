#include "CLineEdit.h"
#include "ui_CLineEdit.h"
#include "CFont.h"

#include <QFileDialog>

CLineEdit::CLineEdit(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::CLineEdit)
{
    ui->setupUi(this);
    ui->clearButton->setObjectName("clearIcon");
    ui->clearButton->setText(CFont::iconRemoveCircle());
    ui->clearButton->hide();
    ui->browseButton->setObjectName("clearIcon");
    ui->browseButton->setText(CFont::iconFolderOpen());
    ui->lineEdit->setObjectName("clearIcon");
    ui->lineEdit->setAttribute(Qt::WA_MacShowFocusRect, 0);
    setBrowseVisible(false);
    mode = QFileDialog::Directory;
}

CLineEdit::~CLineEdit(){ delete ui; }
QLineEdit * CLineEdit::lineEdit(void){ return ui->lineEdit; }
QPushButton * CLineEdit::clearButton(void){ return ui->clearButton; }


void CLineEdit::on_lineEdit_textChanged(const QString &arg1)
{
    if( arg1 != "" ){
        ui->clearButton->show();
    } else {
        ui->clearButton->hide();
    }
}

void CLineEdit::on_clearButton_clicked()
{
    ui->lineEdit->clear();
    emit cleared();
}

void CLineEdit::on_browseButton_clicked()
{
  QFileDialog dialog;
  connect(&dialog, SIGNAL(fileSelected(QString)),
          ui->lineEdit,
          SLOT(setText(QString)));

  connect(&dialog, SIGNAL(fileSelected(QString)), this, SIGNAL(fileDialogSelected()));

  dialog.setAcceptMode(accept);
  dialog.setFileMode(mode);
  dialog.exec();
}

void CLineEdit::setBrowseVisible(bool arg1){
  ui->browseButton->setVisible(arg1);
}

void CLineEdit::setBrowseMode(QFileDialog::FileMode mode,
                              QFileDialog::AcceptMode accept){
  this->mode = mode;
  this->accept = accept;
}
