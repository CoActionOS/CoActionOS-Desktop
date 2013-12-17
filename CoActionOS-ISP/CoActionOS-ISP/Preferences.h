#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QWidget>
#include <CSdk/CLinkPtr.h>


namespace Ui {
class Preferences;
}

class Preferences : public QWidget, public CLinkPtr
{
    Q_OBJECT
    
public:
    explicit Preferences(QWidget *parent = 0);
    ~Preferences();

    void setLink(CLink *d);
    
private slots:
    void connected(bool);
    void on_terminalDeviceRefreshButton_clicked();
    void on_uartParityComboBox_currentIndexChanged(const QString &arg1);
    void on_terminalDeviceComboBox_currentIndexChanged(const QString &arg1);
    void on_terminalDevicePinConfigSpinBox_valueChanged(const QString &arg1);
    void on_uartStopComboBox_currentIndexChanged(const QString &arg1);
    void on_uartBaudrateComboBox_currentIndexChanged(const QString &arg1);
    void on_terminalDeviceLockCheckBox_clicked(bool checked);
    void on_terminalEnableAfterInstallCheckBox_clicked(bool checked);
    void terminalLogFile_editingFinished(void);

private:
    Ui::Preferences *ui;
};

#endif // PREFERENCES_H
