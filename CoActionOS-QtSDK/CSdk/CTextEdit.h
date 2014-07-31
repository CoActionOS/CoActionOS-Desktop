#ifndef CTEXTEDIT_H
#define CTEXTEDIT_H

#include <QTextEdit>
#include <QFile>


class CTextEdit : public QTextEdit
{
    Q_OBJECT

public:
    explicit CTextEdit(QWidget *parent = 0);
    ~CTextEdit();

    void setMaxTextSize(int);
    int maxTextSize(void);

    void appendText(QString text, const QColor & textColor);

    void setSource(QString path);

private:
    int maxSizeInt;

};

#endif // CTEXTEDIT_H
