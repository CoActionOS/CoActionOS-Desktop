/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#ifndef CAOSTERMINAL_H
#define CAOSTERMINAL_H

#include <QWidget>
#include <QTimer>
#include <QString>
#include <CSdk/CLinkPtr.h>

namespace Ui {
    class CaosTerminal;
}

class CaosTerminal : public QWidget, public CLinkPtr
{
    Q_OBJECT

public:
    explicit CaosTerminal(QWidget *parent = 0);
    ~CaosTerminal();

    void setLink(CLink* d);
    bool isOpen(void);

signals:
    void runApplication(QString path);
    void installApplication(void);
    void infoAvailable(void);
    void showPreferences(void);

public slots:
    void closeTerminal();
    void openTerminal();

private slots:
    void linked(bool);
    void readDevice(void);
    void writeDevice(QString);

    void on_runButton_clicked();
    void on_openButton_clicked();
    void on_closeButton_clicked();
    void on_preferencesButton_clicked();
    void on_installButton_clicked();

private:
    Ui::CaosTerminal *ui;
    QTimer readTimer;
    int outFd;
    int inFd;
};

#endif // CAOSTERMINAL_H
