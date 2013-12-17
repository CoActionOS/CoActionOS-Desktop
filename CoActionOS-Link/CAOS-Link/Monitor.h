#ifndef MONITOR_H
#define MONITOR_H

#include <QWidget>
#include <QModelIndex>
#include <QTimer>
#include <QAction>
#include <CSdk/CLinkPtr.h>

namespace Ui {
    class Monitor;
}

class Monitor : public QWidget, public CLinkPtr
{
    Q_OBJECT

public:
    explicit Monitor(QWidget *parent = 0);
    ~Monitor();

    void startTimer(void);
    void stopTimer(void);
    void setLink(CLink *d);


public slots:
    bool isRunningUser(void);

private:
    void updateRow(int tid, void * data);
    void updateItem(int row, int column, QString value);
    QTimer refreshTimer;

private slots:
    void updateAll();
    void on_killButton_clicked();

    void sortByProcessId(void);
    void sortByName(void);
    void sortByCpuUsage(void);
    void sortByMemoryUsage(void);

    void sortAscending(void);
    void sortDescending(void);
    void connected(bool value);

    void on_pauseButton_clicked();

    void on_continueButton_clicked();

    void on_termButton_clicked();

private:
    Ui::Monitor *ui;

    int fd;

    int sortColumn;
    Qt::SortOrder sortOrder;
    QAction * sortAscendingAction;
    QAction * sortDescendingAction;

    int signalProcess(int signo);

    void uncheckAll(void);
    QAction * sortByProcessIdAction;
    QAction * sortByNameAction;
    QAction * sortByCpuUsageAction;
    QAction * sortByMemoryUsageAction;
};

#endif // MONITOR_H
