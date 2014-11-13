/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \file */

/*! \mainpage CoActionOS Link
 *
 *
 * \section INTRO Introduction
 *
 * CoActionOS Link is a graphical application that allows developers to program and debug embedded systems
 * running CoActionOS.
 *
 * \section DEPEND Dependencies
 *
 * CoActionOS Link requires the following packages to build:
 * - Qt from Digia
 * - CoActionOS HWPL Link Library
 * - CoActionOS Qt SDK
 */


#ifndef CAOSLINK_H
#define CAOSLINK_H

#define COACTIONOS_LINK_VERSION "1.6.0"
#include <QMainWindow>
#include <QString>
#include <QProgressBar>
#include <QStatusBar>

namespace Ui {
    class Caoslink;
}

/*! \brief Main Window Class
 * \details This is the class used for the Main Window.
 *
 */
class Caoslink : public QMainWindow
{
    Q_OBJECT

public:
    explicit Caoslink(QWidget *parent = 0);
    ~Caoslink();

    static QString appName(){ return "CoActionOS Link"; }

private:
    Ui::Caoslink *ui;
    void resizeEvent(QResizeEvent * event);

    QString keyMainWindowSize(){ return "KEY_MAINWINDOWSIZE"; }


private slots:
    void notified(int severity);
    void notificationDismissed(int v);

    void installKernelRequest(void);
    void installAppsRequest(void);

};

#endif // CAOSLINK_H
