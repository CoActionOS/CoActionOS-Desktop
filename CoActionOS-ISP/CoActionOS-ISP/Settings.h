#ifndef SETTINGS_H
#define SETTINGS_H

#include <CSdk/CSettings.h>

class Settings : protected CSettings
{
public:
  Settings(QString);

  static QString global(void);

  bool terminalShowAferInstall(void);
  QString terminalDevice(void);
  QString terminalDevicePinConfig(void);
  QString terminalLogFile(void);
  QString uartParity(void);
  QString uartBaudrate(void);
  QString uartStopbits(void);
  QString currentTab(void);

  QString builderWorkspace(void);
  QString builderProject(void);
  bool builderCleanOnMake(void);

  QString ispPioPort0(void);
  QString ispPioPin0(void);
  QString ispPioPort1(void);
  QString ispPioPin1(void);
  QString ispUartPort(void);
  QString ispUartPinAssign(void);
  QString ispFilename(void);

  void setTerminalShowAfterInstall(bool);
  void setTerminalLogFile(QString);
  void setTerminalDevice(QString);
  void setTerminalDevicePinConfig(QString);
  void setUartParity(QString);
  void setUartBaudrate(QString);
  void setUartStopbits(QString);
  void setCurrentTab(QString);

  void setBuilderWorkspace(QString);
  void setBuilderProject(QString);
  void setBuilderCleanOnMake(bool);

  void setIspPioPort0(QString);
  void setIspPioPin0(QString);
  void setIspPioPort1(QString);
  void setIspPioPin1(QString);
  void setIspUartPort(QString);
  void setIspUartPinAssign(QString);
  void setIspFilename(QString);


private:
  typedef enum {
    KEY_TERMINALSHOWAFTERINSTALL,
    KEY_TERMINALDEVICE,
    KEY_TERMINALDEVICEPINCONFIG,
    KEY_TERMINALLOGFILE,
    KEY_UARTPARITY,
    KEY_UARTBAUDRATE,
    KEY_UARTSTOPBITS,
    KEY_CURRENTTAB,
    KEY_BUILDERWORKSPACE,
    KEY_BUILDERPROJECT,
    KEY_BUILDERCLEANONMAKE,
    KEY_ISPPIOPORT0,
    KEY_ISPPIOPORT1,
    KEY_ISPPIOPIN0,
    KEY_ISPPIOPIN1,
    KEY_ISPUARTPORT,
    KEY_ISPUARTPINASSIGN,
    KEY_ISPFILENAME,
    KEY_TOTAL
  } keys;
};

#endif // SETTINGS_H
