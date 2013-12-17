#ifndef ISPTERMINAL_H
#define ISPTERMINAL_H

#include <QWidget>
#include <QTimer>
#include <CSdk/CLinkPtr.h>
#include <applib/Uart.hpp>

namespace Ui {
class IspTerminal;
}

class IspTerminal : public QWidget, public CLinkPtr
{
    Q_OBJECT
    
public:
    explicit IspTerminal(QWidget *parent = 0);
    ~IspTerminal();
    void setLink(CLink *d);

signals:
    void opened(bool);
    void infoAvailable(void);
    void resetTarget(void);
    void showPreferences(void);

public slots:
    void setPaused(bool);

private:
    Ui::IspTerminal *ui;
    QTimer readTimer;
    Uart * uart;

private slots:
    void connected(bool);
    void on_openButton_clicked();
    void on_closeButton_clicked();
    void writeDevice(QString);
    void readDevice(void);
    void on_resetButton_clicked();
};

#endif // ISPTERMINAL_H
