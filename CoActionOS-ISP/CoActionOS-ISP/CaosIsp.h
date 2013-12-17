#ifndef CAOSISP_H
#define CAOSISP_H

#include <QtWidgets/QMainWindow>
#include <QString>

#define COACTIONOS_ISP_VERSION "1.2.8-beta1"

namespace Ui {
class CaosIsp;
}

class CaosIsp : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit CaosIsp(QWidget *parent = 0);
    ~CaosIsp();
    
private slots:
    void on_tabWidget_currentChanged(int index);
    void operationComplete(QString, bool);
    bool installProgram(QString, QString);
    void installCompleted(void);
    void showPreferences(void);

private:
    Ui::CaosIsp *ui;
    QString currentMessage;


    typedef enum {
        TAB_INSTALLER,
        TAB_TERMINAL,
        TAB_PREFERENCES,
        TAB_LABEL
    } TabNames;
};

#endif // CAOSISP_H
