#include "Settings.h"

Settings::Settings(QString name) :
    CSettings(name, QSettings::IniFormat)
{
    addKey("KEY_TERMINALSHOWAFTERINSTALL");
    addKey("KEY_TERMINALDEVICE");
    addKey("KEY_TERMINALDEVICEPINCONFIG");
    addKey("KEY_TERMINALLOGFILE");
    addKey("KEY_UARTPARITY");
    addKey("KEY_UARTBAUDRATE");
    addKey("KEY_UARTSTOPBITS");
    addKey("KEY_CURRENTTAB");
    addKey("KEY_BUILDERWORKSPACE");
    addKey("KEY_BUILDERPROJECT");
    addKey("KEY_BUILDERCLEANONMAKE");
    addKey("KEY_ISPPIOPORT0");
    addKey("KEY_ISPPIOPORT1");
    addKey("KEY_ISPPIOPIN0");
    addKey("KEY_ISPPIOPIN1");
    addKey("KEY_ISPUARTPORT");
    addKey("KEY_ISPUARTPINASSIGN");
    addKey("KEY_ISPFILENAME");
}

QString Settings::global(void){
    QSettings settings;
    return settings.fileName();
}

bool Settings::terminalShowAferInstall(void){ return getBoolKey(KEY_TERMINALSHOWAFTERINSTALL); }
QString Settings::terminalDevice(void){ return getStringKey(KEY_TERMINALDEVICE); }
QString Settings::terminalDevicePinConfig(void){ return getStringKey(KEY_TERMINALDEVICEPINCONFIG); }
QString Settings::terminalLogFile(void){ return getStringKey(KEY_TERMINALLOGFILE); }
QString Settings::uartParity(void){ return getStringKey(KEY_UARTPARITY); }
QString Settings::uartBaudrate(void){ return getStringKey(KEY_UARTBAUDRATE); }
QString Settings::uartStopbits(void){ return getStringKey(KEY_UARTSTOPBITS); }
QString Settings::currentTab(void){ return getStringKey(KEY_CURRENTTAB); }
QString Settings::builderWorkspace(void){ return getStringKey(KEY_BUILDERWORKSPACE); }
QString Settings::builderProject(void){ return getStringKey(KEY_BUILDERPROJECT); }
bool Settings::builderCleanOnMake(void){ return getBoolKey(KEY_BUILDERCLEANONMAKE); }
QString Settings::ispPioPort0(void){ return getStringKey(KEY_ISPPIOPORT0); }
QString Settings::ispPioPin0(void){ return getStringKey(KEY_ISPPIOPIN0); }
QString Settings::ispPioPort1(void){ return getStringKey(KEY_ISPPIOPORT1); }
QString Settings::ispPioPin1(void){ return getStringKey(KEY_ISPPIOPIN1); }
QString Settings::ispUartPort(void){ return getStringKey(KEY_ISPUARTPORT); }
QString Settings::ispUartPinAssign(void){ return getStringKey(KEY_ISPUARTPINASSIGN); }
QString Settings::ispFilename(void){ return getStringKey(KEY_ISPFILENAME); }


void Settings::setTerminalShowAfterInstall(bool arg1){ setKey(KEY_TERMINALSHOWAFTERINSTALL, arg1); }
void Settings::setTerminalDevice(QString arg1){ setKey(KEY_TERMINALDEVICE, arg1); }
void Settings::setTerminalDevicePinConfig(QString arg1){ setKey(KEY_TERMINALDEVICEPINCONFIG, arg1); }
void Settings::setTerminalLogFile(QString arg1){ setKey(KEY_TERMINALLOGFILE, arg1); }
void Settings::setUartParity(QString arg1){ setKey(KEY_UARTPARITY, arg1); }
void Settings::setUartBaudrate(QString arg1){ setKey(KEY_UARTBAUDRATE, arg1); }
void Settings::setUartStopbits(QString arg1){ setKey(KEY_UARTSTOPBITS, arg1); }
void Settings::setCurrentTab(QString arg1){ setKey(KEY_CURRENTTAB, arg1); }
void Settings::setBuilderWorkspace(QString arg1){ setKey(KEY_BUILDERWORKSPACE, arg1); }
void Settings::setBuilderProject(QString arg1){ setKey(KEY_BUILDERPROJECT, arg1); }
void Settings::setBuilderCleanOnMake(bool arg1){ setKey(KEY_BUILDERCLEANONMAKE, arg1); }

void Settings::setIspPioPort0(QString arg1){ setKey(KEY_ISPPIOPORT0, arg1); }
void Settings::setIspPioPin0(QString arg1){ setKey(KEY_ISPPIOPIN0, arg1); }
void Settings::setIspPioPort1(QString arg1){ setKey(KEY_ISPPIOPORT1, arg1); }
void Settings::setIspPioPin1(QString arg1){ setKey(KEY_ISPPIOPIN1, arg1); }
void Settings::setIspUartPort(QString arg1){ setKey(KEY_ISPUARTPORT, arg1); }
void Settings::setIspUartPinAssign(QString arg1){ setKey(KEY_ISPUARTPINASSIGN, arg1); }
void Settings::setIspFilename(QString arg1){ setKey(KEY_ISPFILENAME, arg1); }

