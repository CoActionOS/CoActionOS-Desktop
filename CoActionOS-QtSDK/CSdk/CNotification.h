#ifndef CNOTIFICATION_H
#define CNOTIFICATION_H

#include <QWidget>
#include <QString>

namespace Ui {
class CNotification;
}

class CNotification : public QWidget
{
    Q_OBJECT

public:
    explicit CNotification(QWidget *parent = 0);
    ~CNotification();

    void setWarning(const QString & msg);
    void setError(const QString & msg);
    void setQuestion(const QString & msg);
    void setInfo(const QString & msg);

    enum {
        SEVERITY_LOW,
        SEVERITY_MED,
        SEVERITY_HIGH
    };

signals:
    void activated(int severity);
    void dismissed(int v);

private slots:
    void on_okButton_clicked();
    void on_cancelButton_clicked();

    void on_dismissButton_clicked();

private:
    Ui::CNotification *ui;

    void hideAll(void);
};

#endif // CNOTIFICATION_H
