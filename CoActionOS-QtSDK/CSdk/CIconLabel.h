#ifndef CICONLABEL_H
#define CICONLABEL_H

#include <QWidget>
#include <QLabel>

namespace Ui {
class CIconLabel;
}

class CIconLabel : public QWidget
{
    Q_OBJECT
    
public:
    explicit CIconLabel(QWidget *parent = 0);
    ~CIconLabel();

    void setText(QString icon, QString label);

    QLabel * icon();
    QLabel * label();
    
private:
    Ui::CIconLabel *ui;
};

#endif // CICONLABEL_H
