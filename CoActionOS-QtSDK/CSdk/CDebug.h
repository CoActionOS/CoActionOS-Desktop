#ifndef CDEBUG_H
#define CDEBUG_H

#include <QWidget>
#include <QTimer>
#include <QListWidgetItem>

#include "CLinkPtr.h"

namespace Ui {
class CDebug;
}

class CDebug : public QWidget, public CLinkPtr
{
    Q_OBJECT

public:
    explicit CDebug(QWidget *parent = 0);
    ~CDebug();

    void appendLog(QString text, const QColor & textColor = QColor(Qt::black));
    void setLink(CLink * d);

    void appendDebugMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg);
    void enableMessageHandler();
    void updateStatus(const QString &msg);

    void loadPreferences();
    void savePreferences();

public slots:
    void currentAppUpdated(QString workspace, QString project, QString conf);
    void currentKernelUpdated(QString workspace, QString project, QString conf);

private slots:
    void connected(bool);
    void on_showAssemblyButton_clicked();
    void on_showLogButton_clicked();
    void on_clearButton_clicked();
    void on_showTraceButton_clicked();

    void toggleSettings();

    void on_showSettingsButton_clicked();
    void on_refreshPidButton_clicked();
    void on_startTraceButton_clicked();
    void on_stopTraceButton_clicked();

    void traceTimerExpired(void);
    void on_traceListWidget_itemClicked(QListWidgetItem *item);

private:
    Ui::CDebug *ui;
    void resizeEvent(QResizeEvent * event);

    link_trace_id_t trace_id;
    QTimer traceTimer;
    QString appPath;
    QString kernelPath;

    bool assemblyVisible, logVisible, traceVisible;

};

#endif // CDEBUG_H
