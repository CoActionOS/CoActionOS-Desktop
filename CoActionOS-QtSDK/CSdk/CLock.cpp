#include "CLock.h"

#include "CFont.h"

CLock::CLock(QWidget * parent) : CCheckbox(parent)
{

    this->setMinimumWidth(45);
}


void CLock::refresh(){
    QString icon;
    if( isChecked() ){
        icon = CFont::iconUnlock();
    } else {
        icon = CFont::iconLock();
    }
    CLabel::setText(CFont::fontAwesome(icon));
}
