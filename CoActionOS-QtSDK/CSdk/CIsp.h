#ifndef CISP_H
#define CISP_H

#include <QWidget>
#include <QComboBox>
#include <QTimer>
#include <applib/Uart.hpp>

#include "CLinkPtr.h"


namespace Ui {
class CIsp;
}

class CIsp : public QWidget, public CLinkPtr
{
    Q_OBJECT

public:
    explicit CIsp(QWidget *parent = 0);
    ~CIsp();

signals:
    void installKernel(void);

private slots:
    void connected(bool);
    void on_hideOptionsButton_clicked();
    void on_programButton_clicked();
    void on_resetButton_clicked();
    void on_cancelButton_clicked();
    void on_parityButton_clicked(bool checked);
    void on_startTerminalButton_clicked();
    void readTerminal(void);
    void on_stopBits_clicked(bool checked);
    void on_pauseTerminalButton_clicked();

private:
    static int updateProgress(float prog);
    void programTarget(void);
    void populateComboBox(QString prefix, QComboBox * comboBox);
    void resetTarget(void);
    void resizeEvent(QResizeEvent * event);
    Ui::CIsp *ui;
    bool abort;
    QTimer terminalTimer;
    Uart * terminalUart;
};

#endif // CISP_H
