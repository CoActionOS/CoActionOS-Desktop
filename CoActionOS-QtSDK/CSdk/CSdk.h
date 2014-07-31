#ifndef CSDK_H
#define CSDK_H

#include "CSdk_global.h"
#include <QString>

class CSdk {
public:
    CSdk();

    QString errorMessage(void);

protected:
    void setErrorMessage(QString);

private:
    QString errorMsg;
    static QString appName_;
};

#endif // CSDK_H
