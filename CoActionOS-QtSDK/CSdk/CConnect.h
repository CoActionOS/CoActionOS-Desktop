#ifndef CCONNECT_H
#define CCONNECT_H

#include <QWidget>
#include <QTimer>

#include "CSdk.h"
#include "CLink.h"


namespace Ui {
class CConnect;
}

class CConnect : public QWidget, public CSdk
{
    Q_OBJECT
    
public:
    explicit CConnect(QWidget *parent = 0);
    ~CConnect();

    void refresh(void);
    CLink * clink(){ return &clink_; }

public slots:
    void connectRequested(bool suppressError);

private slots:
    void refreshSerialno(void);
    void connected(bool status);
    void timeout();

    void on_connectButton_clicked();
    void on_refreshButton_clicked();
    void on_disconnectButton_clicked();
    

private:
    void resizeEvent(QResizeEvent * event);
    Ui::CConnect *ui;
    bool errorSuppression;
    CLink clink_;
    QTimer periodic;
};

#endif // CCONNECT_H
