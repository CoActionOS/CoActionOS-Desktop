#include <QThread>

#include <iface/link.h>
#include "SLinkDriver.h"

class SerialPort : public QSerialPort {
public:

    SerialPort(const QString & name) : QSerialPort(name){}
    void lock(){ mutex.lock(); }
    void unlock(){ mutex.unlock(); }

private:
    QMutex mutex;
};

SLinkDriver::SLinkDriver(){

    //initialize the driver
    m_driver.getname = getname;
    m_driver.lock = lock;
    m_driver.unlock = unlock;
    m_driver.status = status;

    m_driver.dev.handle = 0;
    m_driver.dev.open = open;
    m_driver.dev.write = write;
    m_driver.dev.read = read;
    m_driver.dev.close = close;
    m_driver.dev.wait = wait;
    m_driver.dev.flush = flush;
    m_driver.dev.timeout = 250;
}

link_transport_phy_t SLinkDriver::open(const char * name, int baudrate){

    SerialPort * serialPort = new SerialPort(QString(name));
    baudrate = 0; //prevent warnings

    if( serialPort->open(SerialPort::ReadWrite) == true ){
        printf("--------Opened serial port %s\n", name); fflush(stdout);
        return (void*)serialPort;
    } else {
        delete serialPort;
    }

    return LINK_PHY_OPEN_ERROR;
}


int SLinkDriver::getname(char * dest, const char * last, int len){
    QList<QSerialPortInfo> list;
    int i;
    bool pastLast;
    list = QSerialPortInfo::availablePorts();
    pastLast = false;

    if( strlen(last) == 0 ){
        pastLast = true;
    }

    for(i=0; i < list.length(); i++){
        if( list.at(i).portName().contains("Bluetooth") ){
            list.removeAt(i);
            i--;
        } else {
            qDebug() << list.at(i).portName();
        }
    }


    for(i=0; i < list.length(); i++){
        if( pastLast == true ){
            strncpy(dest, list.at(i).portName().toStdString().c_str(), len);
            return 0;
        } else if( strcmp(last, list.at(i).portName().toStdString().c_str()) == 0 ){
            pastLast = true;
        }
    }

    return -1;
}

int SLinkDriver::lock(link_transport_phy_t handle){
    SerialPort * serialPort = (SerialPort *)handle;
    if( serialPort ){
        serialPort->lock();
    }
    return 0;

}

int SLinkDriver::unlock(link_transport_phy_t handle){
    SerialPort * serialPort = (SerialPort *)handle;
    if( serialPort ){
        serialPort->unlock();
    }
    return 0;
}

int SLinkDriver::status(link_transport_phy_t handle){
    SerialPort * serialPort = (SerialPort *)handle;

    if( handle == LINK_PHY_OPEN_ERROR ){
        return LINK_PHY_ERROR;
    }

    if( serialPort->error() != SerialPort::NoError ){
        //serialPort->close();
        //delete serialPort;

        //this might leak memory
        printf("Bad status %d:%s\n", serialPort->error(), serialPort->errorString().toStdString().c_str()); fflush(stdout);
        //return LINK_PHY_ERROR;
    }

    return 0;

}

int SLinkDriver::write(link_transport_phy_t handle, const void* buf, int nbyte){
    int n;
    SerialPort * serialPort = (SerialPort *)handle;
    if( handle == LINK_PHY_OPEN_ERROR ){
        return LINK_PHY_ERROR;
    }
    n = serialPort->write((const char*)buf, nbyte);
    serialPort->flush();
    return n;
}

int SLinkDriver::read(link_transport_phy_t handle, void *buf, int nbyte){
    SerialPort * serialPort = (SerialPort *)handle;
    if( handle == LINK_PHY_OPEN_ERROR ){
        return LINK_PHY_ERROR;
    }
    if( serialPort->waitForReadyRead(0) == false ){
        serialPort->clearError();
    }

    if( serialPort->bytesAvailable() < nbyte ){
        return 0;
    }

    return serialPort->read((char*)buf, nbyte);
}

int SLinkDriver::close(link_transport_phy_t handle){
    SerialPort * serialPort = (SerialPort *)handle;

    if( handle == LINK_PHY_OPEN_ERROR ){
        return LINK_PHY_ERROR;
    }

    serialPort->close();

    delete serialPort;
    return 0;
}

void SLinkDriver::wait(int msec){
    QThread::usleep(msec*1000);
}

void SLinkDriver::flush(link_transport_phy_t handle){
    SerialPort * serialPort = (SerialPort *)handle;
    serialPort->clear(SerialPort::AllDirections);
}
