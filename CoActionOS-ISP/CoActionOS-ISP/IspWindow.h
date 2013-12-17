#ifndef ISPWINDOW_H
#define ISPWINDOW_H

#include <QWidget>

namespace Ui {
class IspWindow;
}

class IspWindow : public QWidget
{
    Q_OBJECT
    
public:
    explicit IspWindow(QWidget *parent = 0);
    ~IspWindow();

signals:
    void openTerminal(void);
    void closeTerminal(void);

private:
    Ui::IspWindow *ui;
};

#endif // ISPWINDOW_H
