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
    static QString currentApplicationProject(void);
    static QString currentKernelProject(void);

    static void setCurrenTab(QString);
    static void setCurrentApplicationProject(QString);
    static void setCurrentKernelProject(QString);

    void setLink(CLink *d);

    void showTerminal(void);




private slots:
    void connected(bool);
    void nohighlights(void);

    void terminalLogFileLineEdit_editingFinished();
    void on_terminalMaximumSizeLockCheckBox_toggled(bool checked);
    void on_terminalAppendModeCheckBox_clicked(bool checked);
    void on_terminalWarnOverwriteCheckBox_clicked(bool checked);
    void on_savePreferencesButton_clicked();
    void savePreferencesButton_complete(QString name);

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
