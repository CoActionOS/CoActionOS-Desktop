#include "CLabel.h"


#include <CSdk/CNotify.h>

CLabel::CLabel(QWidget *parent) :
    QLabel(parent)
{

}

void CLabel::mousePressEvent(QMouseEvent *ev){
    if( checkState_ == Qt::Checked ){
        checkState_ = Qt::Unchecked;
    } else {
        checkState_ = Qt::Checked;
    }
    emit clicked();
    emit clicked(checkState_ == Qt::Checked);
    QLabel::mousePressEvent(ev);
}


void CLabel::setChecked(bool checked){
    if( checked ){
        checkState_ = Qt::Checked;
    } else {
        checkState_ = Qt::Unchecked;
    }
}
