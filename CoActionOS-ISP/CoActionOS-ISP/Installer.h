#ifndef INSTALLER_H
#define INSTALLER_H

#include <QWidget>
#include <CSdk/CLinkPtr.h>

#include <applib/LpcIsp.hpp>


namespace Ui {
class Installer;
}

class Installer : public QWidget, public CLinkPtr
{
    Q_OBJECT
    
public:
    explicit Installer(QWidget *parent = 0);
    ~Installer();

    void setLink(CLink *d);

signals:
    void pauseTerminal(bool);
    void completed(void);

public slots:
    void connected(bool);
    void resetTarget(void);
    bool installProgram(QString path, QString arch);
    
private slots:


    void on_lpcCheckBox_clicked(bool checked);
    void on_stm32CheckBox_clicked(bool checked);

    void on_goButton_clicked();
    void on_cancelButton_clicked();

    void on_serialPinConfigLockCheckBox_clicked(bool checked);

private:
    Ui::Installer *ui;
    Isp * current;
    bool abort;

    static int updateProgress(float prog);


};

#endif // INSTALLER_H
