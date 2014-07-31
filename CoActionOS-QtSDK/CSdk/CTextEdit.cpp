#include "CTextEdit.h"

#include <QScrollBar>
#include <QFile>
#include <QByteArray>

#define MIN_SIZE (10*1024)


CTextEdit::CTextEdit(QWidget *parent) :
    QTextEdit(parent)
{
    maxSizeInt = MIN_SIZE;
}

CTextEdit::~CTextEdit()
{

}

void CTextEdit::setMaxTextSize(int max){
    maxSizeInt = max;
    if( maxSizeInt < MIN_SIZE ){
        maxSizeInt = MIN_SIZE;
    }
}

int CTextEdit::maxTextSize(void){
    return maxSizeInt;
}


void CTextEdit::appendText(QString text, const QColor & textColor){
    int currentScroll;
    int maximumScroll;
    QTextCursor currentCursor;
    QTextCursor insertCursor;

    currentScroll = verticalScrollBar()->value();
    maximumScroll = verticalScrollBar()->maximum();
    currentCursor = textCursor();
    insertCursor = currentCursor;

    //Put the write text in a different color
    insertCursor.movePosition(QTextCursor::End);
    insertCursor.clearSelection();
    setTextCursor(insertCursor);
    //setTextColor(textColor);
    setTextColor(Qt::black);
    insertPlainText(text);

    if ( toPlainText().size() > maxSizeInt ){
        setText( toPlainText().right(maxSizeInt/2) ); //remove half the text
        textCursor().movePosition(QTextCursor::End);
        verticalScrollBar()->setValue( verticalScrollBar()->maximum() );
    }

    setTextCursor(currentCursor);

    if ( currentScroll == maximumScroll ){
        verticalScrollBar()->setValue( verticalScrollBar()->maximum() );
    } else {
        verticalScrollBar()->setValue(currentScroll);
    }

}

void CTextEdit::setSource(QString path){
    //load the s file to the app text area
    //file.setFileName(path);
    path = QString();
}

