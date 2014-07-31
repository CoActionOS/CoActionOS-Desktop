#ifndef CLISTWIDGET_H
#define CLISTWIDGET_H


#include <QListWidget>
#include <QColor>

class CListWidget : public QListWidget
{
    Q_OBJECT

public:
    explicit CListWidget(QWidget *parent = 0);
    ~CListWidget();

    void appendItem(const QString & label);
    void appendItem(const QString & label, const QColor & fg, const QColor & bg);

private:
    int maxSizeInt;
};

#endif // CLISTWIDGET_H
