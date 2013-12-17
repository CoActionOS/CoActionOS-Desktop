#ifndef CKERNELSETTINGS_H
#define CKERNELSETTINGS_H

#include "CSettings.h"

class CKernelSettings : public CSettings
{
public:
  CKernelSettings(QString projectPath, bool create = true);

  static QString settings(void){ return "ckernelsettings.ini"; }

  void setConfigurations(QStringList value){ return setKey(KEY_configurations, value); }
  void setConfiguration(QString value){ return setKey(KEY_configuration, value); }
  void setVerifyInstall(bool value){ return setKey(KEY_verifyinstall, value); }
  void setDownloadSymbols(bool value){ return setKey(KEY_downloadsymbols, value); }
  void setFloatSymbols(bool value){ return setKey(KEY_floatsymbols, value); }
  void setStartAddr(QString value){ return setKey(KEY_startaddr, value); }


  QStringList configurations(){ return getStringListKey(KEY_configurations); }
  QString configuration(){ return getStringKey(KEY_configuration); }
  bool verifyInstall(){ return getBoolKey(KEY_verifyinstall); }
  bool floatSymbols(){ return getBoolKey(KEY_floatsymbols); }
  bool downloadSymbols(){ return getBoolKey(KEY_downloadsymbols); }
  QString startAddr(){ return getStringKey(KEY_startaddr); }

private:
  enum {
    KEY_configurations,
    KEY_configuration,
    KEY_verifyinstall,
    KEY_floatsymbols,
    KEY_startaddr,
    KEY_downloadsymbols
  };

};

#endif // CKERNELSETTINGS_H
