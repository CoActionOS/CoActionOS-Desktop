#include "CCheckbox.h"
#include "CFont.h"

CCheckbox::CCheckbox(QWidget * parent) : CLabel(parent){
    connect(this, SIGNAL(clicked()), this, SLOT(on_clicked()));

    this->setAlignment(Qt::AlignLeft);
}


void CCheckbox::setText(const QString & text){
    text_ = text;
    refresh();
}

void CCheckbox::refresh(){
    QString icon;
    if( isChecked() ){
        icon = CFont::iconCheck();
    } else {
        icon = CFont::iconCheckEmpty();
    }
    CLabel::setText(CFont::fontAwesome(icon) + " " + text_);
}

void CCheckbox::setCheckState(enum Qt::CheckState state){
    CLabel::setCheckState(state);
    refresh();
}



void CCheckbox::on_clicked(){
    refresh();
    emit clicked(isChecked());
}
