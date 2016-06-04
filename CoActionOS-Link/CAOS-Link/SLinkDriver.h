#ifndef SLINKDRIVER_H
#define SLINKDRIVER_H


#include <iface/link_transport.h>
#include <QMutex>
#include <QtSerialPort/QtSerialPort>

class SLinkDriver {
public:
    SLinkDriver();
    link_transport_mdriver_t * driver(){ return &m_driver; }

private:

    static int getname(char * dest, const char * last, int len);
    static int lock(link_transport_phy_t handle);
    static int unlock(link_transport_phy_t handle);
    static int status(link_transport_phy_t handle);

    static link_transport_phy_t open(const char *, int);
    static int write(link_transport_phy_t, const void*, int);
    static int read(link_transport_phy_t, void*, int);
    static int close(link_transport_phy_t);
    static void wait(int);
    static void flush(link_transport_phy_t);

    link_transport_mdriver_t m_driver;

};

#endif // SLINKDRIVER_H
