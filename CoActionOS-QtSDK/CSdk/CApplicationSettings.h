#ifndef CAPPLICATIONSETTINGS_H
#define CAPPLICATIONSETTINGS_H

#include "CSettings.h"

class CApplicationSettings : public CSettings
{
public:
  CApplicationSettings(QString projectPath, bool create = true);

  static QString settings(void){ return "cappsettings.ini"; }

  QString configuration(){ return getStringKey(KEY_configuration); }
  QStringList configurations(){ return getStringListKey(KEY_configurations); }
  bool uploadSymbols(){ return getBoolKey(KEY_uploadSymbols); }
  QString dataSize(){ return getStringKey(KEY_dataSize); }
  QString installPath(){ return getStringKey(KEY_installPath); }
  QString kernelReference(){ return getStringKey(KEY_kernelreference); }

  bool startup(){ return getBoolKey(KEY_startup); }
  bool run(){ return getBoolKey(KEY_run); }
  bool ram(){ return getBoolKey(KEY_ram); }

  void setDataSize(int size){ setKey(KEY_dataSize, QString::number(size)); }
  void setInstallPath(QString path){ setKey(KEY_installPath, path); }
  void setKernelReference(QString path){ setKey(KEY_kernelreference, path); }
  void setConfiguration(QString configuration){ setKey(KEY_configuration, configuration); }
  void setConfigurations(QStringList configurations){ setKey(KEY_configurations, configurations); }
  void setStartup(bool value){ setKey(KEY_startup, value); }
  void setRun(bool value){ setKey(KEY_run, value); }
  void setRam(bool value){ setKey(KEY_ram, value); }
  void setUploadSymbols(bool value){ setKey(KEY_uploadSymbols, value); }

private:
  enum{
    KEY_configuration,
    KEY_configurations,
    KEY_installPath,
    KEY_uploadSymbols,
    KEY_kernelreference,
    KEY_dataSize,
    KEY_startup,
    KEY_run,
    KEY_ram,
    KEY_total
  };


  static const QString keys[KEY_total];


};

#endif // CAPPLICATIONSETTINGS_H
