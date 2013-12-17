/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#ifndef ASSEMBLYPARSER_H
#define ASSEMBLYPARSER_H

#include <QString>
#include <QFile>
#include <QList>

#include <stdint.h>

class AssemblyParser
{
public:
    AssemblyParser();
    ~AssemblyParser();
    int buildSymbols(QString source, QString dest, bool useFloat);
    int addEabiAliases(QFile * symbols);
    QString getErrorMessage();
    static uint32_t calcSignature(QString source);

    enum {
        INVALID_SIGNATURE = 0xFFFFFFFF
    };

private:
    QString addAlias(QString symbol, bool useFloat);
    bool strictFilter(QString symbol);
    QString errorMessage;
    QFile strictFilterFile;
    bool strictFilterEnabled;
    QList<QString> symbolList;
};

#endif // ASSEMBLYPARSER_H
