#include "CSdk.h"


CSdk::CSdk()
{
   errorMsg = "";
}


QString CSdk::errorMessage(void){
    return errorMsg;
}


void CSdk::setErrorMessage(QString msg){
    errorMsg = msg;
}
