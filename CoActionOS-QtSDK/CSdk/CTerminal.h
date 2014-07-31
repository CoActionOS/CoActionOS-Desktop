#ifndef CTERMINAL_H
#define CTERMINAL_H

#include <QtWidgets/QWidget>
#include <QTimer>
#include <QString>
#include <QFile>

namespace Ui {
class CTerminal;
}

class CTerminal : public QWidget
{
    Q_OBJECT
    
public:
    explicit CTerminal(QWidget *parent = 0);
    ~CTerminal();

    void setMaxTextSize(int);
    int maxTextSize(void);
    void setLogFileName(QString name, bool append = false, bool overwriteWarn = true);
    bool isLogOpen(void);

signals:
    void sendText(QString); //send text to the device

public slots:
    void receiveText(QString);
    void opened(bool);

private:
    Ui::CTerminal *ui;
    //void addText(QString text, const QColor & textColor);
    int maxSizeInt;
    QString logFileNameVar;
    QFile logFile;
    bool logWarnOverwriteVar;
    bool logAppendVar;


private slots:
    void on_sendButton_clicked();
    void on_clearButton_clicked();
};

#endif // CTERMINAL_H
