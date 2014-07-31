#ifndef CLINK_H
#define CLINK_H

#include <QApplication>
#include <QString>
#include <QStringList>
#include <ctime>
#include <hwpl/Link.h>

class CLink : public QObject, public Link
{
    Q_OBJECT
public:
    CLink() : Link(){}
    bool isConnected(){ return connected(); }

    static void setDebug(int value){ link_set_debug(value); }
    static void setDebugCallback(void (*callback)(link_debug_context_t*)){
        link_set_debug_callback(callback);
    }

    QStringList dirList(QString dir){
        QStringList ret;
        int dirp;
        struct link_dirent entry;
        struct link_dirent * result;
        dirp = opendir(dir.toStdString());

        ret.clear();
        if( dirp == 0 ){
            //failed
            return ret;
        }

        while( readdir_r(dirp, &entry, &result) == 0 ){
            if( result == NULL ){
                break;
            }
            ret.append( QString(entry.d_name) );
        }
        closedir(dirp);
        return ret;
    }

    int getNumDevices(void){
        vector<string> list;
        list = Link::listDevices();
        return list.size();
    }

    int init(string sn){
        int ret = Link::init(sn);
        if( ret == 0 ){
            emit linked(true);
        }
        qApp->processEvents();
        return ret;
    }

    int reinit(){
        int ret = Link::reinit();
        if( ret == 0 ){
            emit linked(true);
        }
        qApp->processEvents();
        return ret;
    }

    int exit(){
        emit linked(false);
        qApp->processEvents();
        return Link::exit();
    }

    int reset(){
        emit linked(false);
        qApp->processEvents();
        return Link::reset();
    }

    int resetBootloader(){
        emit linked(false);
        qApp->processEvents();
        return Link::resetBootloader();
    }


signals:
    void linked(bool isConnected);
    void aboutToDisconnect(void);
    void reconnectSignal(bool suppressError);

};

#endif // CLINK_H
