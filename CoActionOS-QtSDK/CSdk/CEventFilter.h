#ifndef CEVENTFILTER_H
#define CEVENTFILTER_H


#include <QObject>
#include <QEvent>

class CEventFilter : public QObject
{
    Q_OBJECT

public:
    CEventFilter(QEvent::Type type){
        type_ = type;
    }

protected:
    bool eventFilter(QObject * obj, QEvent * event){
        if( event->type() == type_ ){
            return true;
        }
        return QObject::eventFilter(obj, event);
    }

private:
    QEvent::Type type_;


};

#endif // CEVENTFILTER_H
