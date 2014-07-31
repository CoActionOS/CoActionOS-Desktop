
#include <QScrollBar>
#include <QListWidgetItem>
#include "CListWidget.h"



CListWidget::CListWidget(QWidget *parent) : QListWidget(parent)
{
    maxSizeInt = 500;
}

CListWidget::~CListWidget()
{

}



void CListWidget::appendItem(const QString & label, const QColor & fg, const QColor & bg){
    QListWidgetItem * item;
    int currentScroll;
    int maximumScroll;
    int newRow;

    currentScroll = verticalScrollBar()->value();
    maximumScroll = verticalScrollBar()->maximum();

    newRow = count();
    this->addItem(label);

    this->item(newRow)->setBackground(bg);
    this->item(newRow)->setForeground(fg);

    if( this->count() > maxSizeInt ){
        item = this->takeItem(0);
        if( item != 0 ){
            delete item;
        }
    }

    if ( currentScroll == maximumScroll ){
        verticalScrollBar()->setValue( verticalScrollBar()->maximum() );
    } else {
        verticalScrollBar()->setValue(currentScroll);
    }
}

void CListWidget::appendItem(const QString & label){
    appendItem(label, Qt::black, Qt::white);
}
