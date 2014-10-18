/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#ifndef CAOSINTERFACE_H
#define CAOSINTERFACE_H

#include <QWidget>
#include <QTimer>
#include <hwpl/Link.h>
#include <CSdk/CLinkPtr.h>
#include <CSdk/CDebug.h>
#include <CSdk/CEventFilter.h>

namespace Ui {
class CaosInterface;
}

class CaosInterface : public QWidget, public CLinkPtr
{
    Q_OBJECT

public:
    explicit CaosInterface(QWidget *parent = 0);
    ~CaosInterface();

    void refresh(void);

    enum {
        APPLICATION_TAB_INDEX = 0,
        KERNEL_TAB_INDEX,
        ISP_TAB_INDEX,
        FILEBROWSER_TAB_INDEX,
        TERMINAL_TAB_INDEX,
        DEBUG_TAB_INDEX,
        MONITOR_TAB_INDEX,
        PREFERENCES_TAB_INDEX,
        ABOUT_TAB_INDEX
    };

    void setLink(CLink * device);
    CDebug * debug();

    void installKernelRequest(void);
    void installAppsRequest(void);

signals:
    void updateProgress(QString msg, int progress, int max);
    void operationComplete(QString,bool);
    void updateStatusMessage(QString msg);
    void error(QString msg);
    void init(void);

    void installKernel(void);
    void installApps(void);

private slots:

    void on_tabs_currentChanged(int index);
    void runApplicationInTerminal(QString path);

    void linked(bool connected);
    void updateInfo(void);

    void consoleInfoAvailable(void);
    void terminalInfoAvailable(void);

    void showPreferences(void);
    void showKernelPreferences(void);
    void showApplicationPreferences(void);
    void showTerminalPreferences(void);
    void showKernel(void);

    void applicationUpdated(QString workspace, QString project, QString conf);
    void kernelUpdated(QString workspace, QString project, QString conf);

private:
    Ui::CaosInterface *ui;
    QString lastApp;
    QTimer infoTimer;
    bool consoleInfo;
    bool terminalInfo;

    void checkCloseTerminal(void);
    void checkOpenTerminal(void);
};

#endif // CAOSINTERFACE_H
