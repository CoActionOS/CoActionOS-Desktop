#ifndef CLABEL_H
#define CLABEL_H

#include <QLabel>

class CLabel : public QLabel
{
    Q_OBJECT
public:
    explicit CLabel(QWidget *parent = 0);

    void setChecked(bool checked = true);
    bool isChecked(void) const { return checkState_ == Qt::Checked; }
    Qt::CheckState checkState(void) const { return checkState_; }

signals:
    void clicked(bool);
    void clicked(void);

public slots:

protected:
    //bool event(QEvent *e){ QLabel::event(e); }
    //void keyPressEvent(QKeyEvent *ev);
    //void paintEvent(QPaintEvent *);
    //void changeEvent(QEvent *);
    void mousePressEvent(QMouseEvent *ev);
    //void mouseMoveEvent(QMouseEvent *ev);
    //void mouseReleaseEvent(QMouseEvent *ev);
    //void contextMenuEvent(QContextMenuEvent *ev);
    //void focusInEvent(QFocusEvent *ev);
    //void focusOutEvent(QFocusEvent *ev);
    //bool focusNextPrevChild(bool next);

private:
    Qt::CheckState checkState_;

private slots:

};

#endif // CLABEL_H
