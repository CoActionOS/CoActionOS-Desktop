#ifndef ABOUT_H
#define ABOUT_H

#include <QWidget>
#include <QTimer>
#include <CSdk/CLinkPtr.h>

namespace Ui {
    class About;
}

#include <QString>

class About : public QWidget, public CLinkPtr
{
    Q_OBJECT

public:
    explicit About(QWidget *parent = 0);
    ~About();

    void setLink(CLink *d);

public slots:
    void connected(bool);

private slots:
    void updateTimeOfDay(void);


    void on_coactionosTimeButton_clicked();

private:
    void refreshKernel();
    Ui::About *ui;
    QTimer timeTimer;

};

#endif // ABOUT_H
