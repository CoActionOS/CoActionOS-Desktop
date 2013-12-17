#ifndef CLINEEDIT_H
#define CLINEEDIT_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>

namespace Ui {
class CLineEdit;
}

class CLineEdit : public QFrame
{
    Q_OBJECT
    
public:
    explicit CLineEdit(QWidget *parent = 0);
    ~CLineEdit();
    QLineEdit * lineEdit(void);
    QPushButton * clearButton(void);

    void setBrowseVisible(bool arg1);
    void setBrowseMode(QFileDialog::FileMode mode, QFileDialog::AcceptMode accept = QFileDialog::AcceptOpen);

signals:
    void cleared(void);
    void fileDialogSelected(void);
    
private slots:
    void on_lineEdit_textChanged(const QString &arg1);
    void on_clearButton_clicked();
    void on_browseButton_clicked();

private:
    Ui::CLineEdit *ui;
    QFileDialog::FileMode mode;
    QFileDialog::AcceptMode accept;
};

#endif // CLINEEDIT_H
