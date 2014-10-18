#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QWidget>
#include <QString>
#include <QLabel>
#include <QLineEdit>
#include <QTimer>
#include <CSdk/CLinkPtr.h>
#include <CSdk/CSettings.h>

namespace Ui {
    class Preferences;
}

class Preferences : public QWidget, public CLinkPtr
{
    Q_OBJECT

public:
    explicit Preferences(QWidget *parent = 0);
    ~Preferences();

    static QString terminalMaximumSize(void);
    static QString terminalLogFile(void);
    static bool terminalAppendMode(void);
    static bool terminalWarnOverwrite(void);
    static QString currentTab(void);
    static QString applicationProject(void);
    static QString applicationConfiguration(void);
    static QString kernelProject(void);
    static QString kernelConfiguration(void);


    static QSize windowSize(void);

    static void setCurrenTab(QString);
    static void setApplicationProject(QString);
    static void setApplicationConfiguration(QString);
    static void setkernelProject(QString);
    static void setkernelConfiguration(QString);
    static void setWindowSize(int w, int h);

    void setLink(CLink *d);
    void showTerminal(void);

private slots:
    void connected(bool);
    void nohighlights(void);

    void terminalLogFileLineEdit_editingFinished();
    void on_terminalAppendModeCheckBox_clicked(bool checked);
    void on_terminalWarnOverwriteCheckBox_clicked(bool checked);
    void on_savePreferencesButton_clicked();
    void savePreferencesButton_complete(QString name);

    void terminalMaximumSizeToggled(bool checked);


private:
    Ui::Preferences *ui;

    void startHiglightTimer(void);

    bool initialized;
    QTimer highlightTimer;
    int lastMaxSize;

    void loadPreferences(void);

    CSettings settings;

};

#endif // PREFERENCES_H
