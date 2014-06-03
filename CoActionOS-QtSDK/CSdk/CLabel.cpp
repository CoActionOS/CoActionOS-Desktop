#include "CLabel.h"


#include <CSdk/CNotify.h>

CLabel::CLabel(QWidget *parent) :
    QLabel(parent)
{

}

void CLabel::mousePressEvent(QMouseEvent *ev){
    if( checkState() == Qt::Checked ){
        setChecked(Qt::Unchecked);
    } else {
        setChecked(Qt::Checked);
    }
    emit clicked();
    emit clicked(checkState() == Qt::Checked);
    QLabel::mousePressEvent(ev);
}


void CLabel::setChecked(bool checked){
    if( checked ){
        setCheckState(Qt::Checked);
    } else {
        setCheckState(Qt::Unchecked);
    }
}

void CLabel::setCheckState(enum Qt::CheckState state){
    checkState_ = state;
}

