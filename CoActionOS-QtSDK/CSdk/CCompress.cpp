#include "CCompress.h"

#include <QFile>

CCompress::CCompress()
{
}

int CCompress::compress(QString dest, QString src){
    QFile input;
    QFile output;
    QByteArray dataIn;
    QByteArray dataOut;
    QByteArray line;

    input.setFileName(src);
    if ( input.open(QIODevice::ReadOnly) == false ){
        setErrorMessage("Failed to open file " + src);
        return -1;
    }
    dataIn.clear();
    dataIn = input.readAll();
    qDebug("Compress Data In is %d bytes", dataIn.size());
    input.close();
    dataOut = qCompress(dataIn);
    qDebug("Compress Data out is %d bytes", dataOut.size() );
    output.setFileName(dest);
    if ( output.open(QIODevice::WriteOnly) == false ){
        setErrorMessage("Failed to open file " + dest);
        return -1;
    }
    output.write(dataOut);
    output.close();
    return 0;
}

int CCompress::decompress(QString dest, QString src){
    QFile input;
    QFile output;
    QByteArray dataIn;
    QByteArray dataOut;

    input.setFileName(src);
    if ( input.open(QIODevice::ReadOnly) == false ){
        setErrorMessage("Failed to open file " + src);
        return -1;
    }
    dataIn.clear();
    dataIn = input.readAll();
    qDebug("Decompress Data In is %d bytes", dataIn.size());
    input.close();
    dataOut = qUncompress(dataIn);
    qDebug("Decompress Data out is %d bytes", dataOut.size());
    output.setFileName(dest);
    if ( output.open(QIODevice::WriteOnly) == false ){
        setErrorMessage("Failed to open file " + dest);
        return -1;
    }
    output.write(dataOut);
    output.close();
    return 0;
}
