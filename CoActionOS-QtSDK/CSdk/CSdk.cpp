#include "CSdk.h"


QString CSdk::appName_;

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
