/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include <cstdio>


#include "caoslink.h"
#include "assemblyparser.h"



AssemblyParser::AssemblyParser(){
    //char buffer[64];

    /*
    if ( caoslink->prefs()->getStrictSymbolFiltering() == true ){
        strictFilterFile.setFileName(":data/symbols.txt");
        strictFilterEnabled = strictFilterFile.open(QIODevice::ReadOnly);
        if ( strictFilterEnabled == true ){
            while( ( strictFilterFile.readLine(buffer, 64)) > 0 ){
                symbolList.append( QString(buffer).remove("\n") );
            }
            strictFilterFile.close();
            qSort(symbolList);
        }
    } else {
        strictFilterEnabled = false;
    }
    */

    strictFilterEnabled = false;
}

AssemblyParser::~AssemblyParser(){
}

QString AssemblyParser::getErrorMessage(){
    return errorMessage;
}



uint32_t AssemblyParser::calcSignature(QString source){
    QFile libcaos;
    uint32_t signature;
    uint32_t buf[256];
    int bytesRead;
    int i;
    int bytesCalculated;

    libcaos.setFileName(source);
    if ( libcaos.open(QIODevice::ReadOnly) == false ){
        return -1;
    }

    bytesCalculated = 0;
    signature = 0;
    while( (bytesRead = libcaos.read((char*)buf, 256*sizeof(uint32_t))) > 0 ){
        for(i=0; i < bytesRead / 4; i++){
            signature += buf[i];
            bytesCalculated += 4;
        }
    }

    qDebug("Signature 0x%X on %d bytes\n", signature, bytesCalculated);

    return signature;
}

int AssemblyParser::buildSymbols(QString source, QString dest, bool useFloat){
    QFile assembly;
    QFile symbols;
    QString symbol;
    QString addr;
    QString line;
    QString lineBuffer;
    int mode;
    char buf[1024];
    char * ptr;
    char * symbolStart;
    char * symbolStop;
    qint64 lineLength;

    assembly.setFileName(source);
    if ( assembly.open(QIODevice::ReadOnly) == false ){
        errorMessage = "Failed to open file " + source;
        return -1;
    }

    symbols.setFileName(dest);
    if ( symbols.open(QIODevice::WriteOnly) == false ){
        assembly.close();
        errorMessage = "Failed to create file " + dest;
        return -1;
    }

    mode = 1;
    while ( (lineLength = assembly.readLine(buf, sizeof(buf)) ) > 0  ){
        ptr = buf;
        lineBuffer = QString::fromAscii(buf);
        if ( (lineBuffer == "Disassembly of section .text:\n") ||
             (lineBuffer == "Disassembly of section .text:\r\n") ){
            qDebug("Text Section\n");
            mode = 1;
        } else if ( (lineBuffer == "Disassembly of section .data:\n") ||
                    (lineBuffer == "Disassembly of section .data:\r\n") ){
            qDebug("Data Section\n");
            mode = 0;
        }
        //Search for a Symbol definition
        symbolStart = NULL;
        symbolStop = NULL;
        while( (*ptr != 0) && (symbolStop == NULL) ){
            //Mark the beginning of the symbol
            if (*ptr == '<' ){
                symbolStart = ptr+1;
            }

            //Mark the end of the symbol
            if ( *ptr == '>' ){
                if ( *(ptr+1) == ':' ){
                    symbolStop = ptr;
                }
            }
            ptr++;
        }

        if ( (symbolStop != NULL) && (symbolStart != NULL) ){

            *(symbolStart - 2) = 0;
            *(symbolStop) = 0;
            addr.clear();
            addr.append(buf);

            symbol.clear();
            symbol.append(symbolStart);

            if ( mode ){
                //! \todo Filter more symbols that won't be used by applications

                if ( strictFilter(symbol) == true ){
                    if (symbol != "main"){
                        line = symbol + " = 0x" + QString::number((addr.toInt(0, 16) + mode), 16) + ";\n";
                        line += addAlias(symbol, useFloat);
                        symbols.write(line.toAscii().constData());
                    }
                }
            }

            if ( symbol == "_global_impure_ptr" ){
                line = symbol + " = 0x" + QString::number((addr.toInt(0, 16) + mode), 16) + ";\n";
                symbols.write(line.toAscii().constData());
                line = QString("_impure_ptr") + " = 0x" + QString::number((addr.toInt(0, 16) + mode - 4), 16) + ";\n";
                symbols.write(line.toAscii().constData());
            }

        }

    }

    //addEabiAliases(&symbols);

    errorMessage = "Done";
    assembly.close();
    symbols.close();
    return 0;
}

bool AssemblyParser::strictFilter(QString symbol){

    QList<QString>::iterator it;
    if ( strictFilterEnabled == false ){
        return true;
    }

    it = qBinaryFind( symbolList.begin(), symbolList.end(), symbol);
    if ( it != symbolList.end() ){
        return true;
    }

    if ( symbol == "kill" ){
        qDebug("NO KILL");
    }

    return false;
}

QString AssemblyParser::addAlias(QString symbol, bool useFloat){
    QString alias;

    if ( useFloat == false ){
        if ( symbol == "iprintf" ){ return "printf = iprintf;\n"; }
        if ( symbol == "viprintf" ){ return "vprintf = viprintf;\n"; }
        if ( symbol == "fiprintf" ){ return "fprintf = fiprintf;\n"; }
        if ( symbol == "vfiprintf" ){ return "vfprintf = vfiprintf;\n"; }
        if ( symbol == "siprintf" ){ return "sprintf = siprintf;\n"; }
        if ( symbol == "sniprintf" ){ return "snprintf = sniprintf;\n"; }
        if ( symbol == "vsiprintf" ){ return "vsprintf = vsiprintf;\n"; }
        if ( symbol == "vsniprintf" ){ return "vsnprintf = vsniprintf;\n"; }
        if ( symbol == "iscanf" ){ return "scanf = iscanf;\n"; }
        if ( symbol == "fiscanf" ){ return "fscanf = fiscanf;\n"; }
        if ( symbol == "vfiscanf" ){ return "vfscanf = vfiscanf;\n"; }
        if ( symbol == "siscanf" ){ return "sscanf = siscanf;\n"; }
        if ( symbol == "vsiscanf" ){ return "vsscanf = vsiscanf;\n"; }
    } else {
        if ( symbol == "__aeabi_fneg" ){ return "__negsf2 = __aeabi_fneg;\n"; }
        if ( symbol == "__aeabi_fsub" ){ return "__subsf3 = __aeabi_fsub;\n"; }
        //if ( symbol == "__aeabi_fadd" ){ return "__addsf3 = __aeabi_fadd;\n"; }
        if ( symbol == "__aeabi_ui2f" ){ return "__floatunsisf = __aeabi_ui2f;\n"; }
        if ( symbol == "__aeabi_i2f" ){ return "__floatsisf = __aeabi_i2f;\n"; }
        if ( symbol == "__aeabi_ul2f" ){ return "__floatundisf = __aeabi_ul2f;\n"; }
        if ( symbol == "__aeabi_l2f" ){ return "__floatdisf = __aeabi_l2f;\n"; }
        if ( symbol == "__aeabi_fmul" ){ return "__mulsf3 = __aeabi_fmul;\n"; }
        if ( symbol == "__aeabi_fdiv" ){ return "__divsf3 = __aeabi_fdiv;\n"; }
        if ( symbol == "__aeabi_fcmpun" ){ return "__unordsf2 = __aeabi_fcmpun;\n"; }
        if ( symbol == "__aeabi_f2iz" ){ return "__fixsfsi = __aeabi_f2iz;\n"; }
        if ( symbol == "__aeabi_f2uiz" ){ return "__fixunssfsi = __aeabi_f2uiz;\n"; }
        //if ( symbol == "__aeabi_f2lz" ){ return "__fixsfdi = __aeabi_f2lz;\n"; }
        //if ( symbol == "__aeabi_f2ulz" ){ return "__fixunssfdi = __aeabi_f2ulz;\n"; }
        if ( symbol == "__aeabi_dneg" ){ return "__negdf2 = __aeabi_dneg;\n"; }
        if ( symbol == "__aeabi_dsub" ){ return "__subdf3 = __aeabi_dsub;\n"; }
        //if ( symbol == "__aeabi_dadd" ){ return "__adddf3 = __aeabi_dadd;\n"; }
        if ( symbol == "__aeabi_ui2d" ){ return "__floatunsidf = __aeabi_ui2d;\n"; }
        if ( symbol == "__aeabi_i2d" ){ return "__floatsidf = __aeabi_i2d;\n"; }
        if ( symbol == "__aeabi_f2d" ){ return "__extendsfdf2 = __aeabi_f2d;\n"; }
        if ( symbol == "__aeabi_d2f" ){ return "__truncdfsf2 = __aeabi_d2f;\n"; }
        if ( symbol == "__aeabi_ul2d" ){ return "__floatundidf = __aeabi_ul2d;\n"; }
        if ( symbol == "__aeabi_l2d" ){ return "__floatdidf = __aeabi_l2d;\n"; }
        if ( symbol == "__aeabi_dmul" ){ return "__muldf3 = __aeabi_dmul;\n"; }
        if ( symbol == "__aeabi_ddiv" ){ return "__divdf3 = __aeabi_ddiv;\n"; }
        if ( symbol == "__aeabi_dcmpun" ){ return "__unorddf2 = __aeabi_dcmpun;\n"; }
        if ( symbol == "__aeabi_d2iz" ){ return "__fixdfsi = __aeabi_d2iz;\n"; }
        if ( symbol == "__aeabi_d2uiz" ){ return "__fixunsdfsi = __aeabi_d2uiz;\n"; }
        //if ( symbol == "__aeabi_d2lz" ){ return "__fixdfdi = __aeabi_d2lz;\n"; }
        //if ( symbol == "__aeabi_d2ulz" ){ return "__fixunsdfdi = __aeabi_d2ulz;\n"; }
        //if ( symbol == "__gnu_h2f_ieee" ){ return "__extendhfsf2 = __gnu_h2f_ieee;\n"; }
        //if ( symbol == "__gnu_f2h_ieee" ){ return "__truncsfhf2 = __gnu_f2h_ieee;\n"; }

        if ( symbol == "__cmpsf2" ){ return "__nesf2 = __cmpsf2;\n__eqsf2 = __cmpsf2;\n"; }
        if ( symbol == "__lesf2" ){ return "__ltsf2 = __lesf2;\n"; }
        //if ( symbol == "__aeabi_fcmple" ){ return "__lesf2 = __aeabi_fcmple;\n"; }
        //if ( symbol == "__aeabi_fcmpge" ){ return "__gesf2 = __aeabi_fcmpge;\n"; }
        if ( symbol == "__gesf2" ){ return "__gtsf2 = __gesf2;\n"; }

        if ( symbol == "__cmpdf2" ){ return "__nedf2 = __cmpdf2;\n__eqdf2 = __cmpdf2;\n"; }
        if ( symbol == "__ledf2" ){ return "__ltdf2 = __ledf2;\n"; }
        //if ( symbol == "__aeabi_dcmple" ){ return "__ledf2 = __aeabi_dcmple;\n"; }
        //if ( symbol == "__aeabi_dcmpge" ){ return "__gedf2 = __aeabi_dcmpge;\n"; }
        if ( symbol == "__gedf2" ){ return "__gtdf2 = __gedf2;\n"; }
    }

    alias.clear();

    return alias;
}

int AssemblyParser::addEabiAliases(QFile * symbols){
    QString line;
    line.clear();

    line += "__negsf2 = __aeabi_fneg;\n";
    line += "__subsf3 = __aeabi_fsub;\n";
    //line += "__addsf3 = __aeabi_fadd;\n";
    line += "__floatunsisf = __aeabi_ui2f;\n";
    line += "__floatsisf = __aeabi_i2f;\n";
    line += "__floatundisf = __aeabi_ul2f;\n";
    line += "__floatdisf = __aeabi_l2f;\n";
    line += "__mulsf3 = __aeabi_fmul;\n";
    line += "__divsf3 = __aeabi_fdiv;\n";
    line += "__unordsf2 = __aeabi_fcmpun;\n";
    line += "__fixsfsi = __aeabi_f2iz;\n";
    line += "__fixunssfsi = __aeabi_f2uiz;\n";
    //line += "__fixsfdi = __aeabi_f2lz;\n";
    //line += "__fixunssfdi = __aeabi_f2ulz;\n";
    line += "__negdf2 = __aeabi_dneg;\n";
    line += "__subdf3 = __aeabi_dsub;\n";
    //line += "__adddf3 = __aeabi_dadd;\n";
    line += "__floatunsidf = __aeabi_ui2d;\n";
    line += "__floatsidf = __aeabi_i2d;\n";
    line += "__extendsfdf2 = __aeabi_f2d;\n";
    line += "__truncdfsf2 = __aeabi_d2f;\n";
    line += "__floatundidf = __aeabi_ul2d;\n";
    line += "__floatdidf = __aeabi_l2d;\n";
    line += "__muldf3 = __aeabi_dmul;\n";
    line += "__divdf3 = __aeabi_ddiv;\n";
    line += "__unorddf2 = __aeabi_dcmpun;\n";
    line += "__fixdfsi = __aeabi_d2iz;\n";
    line += "__fixunsdfsi = __aeabi_d2uiz;\n";
    //line += "__fixdfdi = __aeabi_d2lz;\n";
    //line += "__fixunsdfdi = __aeabi_d2ulz;\n";
    //line += "__extendhfsf2 = __gnu_h2f_ieee;\n";
    //line += "__truncsfhf2 = __gnu_f2h_ieee;\n";

    line += "__nesf2 = __cmpsf2;\n";
    line += "__eqsf2 = __cmpsf2;\n";
    line += "__ltsf2 = __lesf2;\n";
    //line += "__lesf2 = __aeabi_fcmple;\n";
    //line += "__gesf2 = __aeabi_fcmpge;\n";
    line += "__gtsf2 = __gesf2;\n";

    line += "__nedf2 = __cmpdf2;\n";
    line += "__eqdf2 = __cmpdf2;\n";
    line += "__ltdf2 = __ledf2;\n";
    //line += "__ledf2 = __aeabi_dcmple;\n";
    //line += "__gedf2 = __aeabi_dcmpge;\n";
    line += "__gtdf2 = __gedf2;\n";

    symbols->write(line.toAscii().constData());
    return 0;
}
