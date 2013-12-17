#include "CApplicationSettings.h"

CApplicationSettings::CApplicationSettings(QString projectPath, bool create) :
  CSettings(projectPath + "/cappsettings.ini", create)
{
  addKey("KEY_configuration");
  addKey("KEY_configurations");
  addKey("KEY_installPath");
  addKey("KEY_uploadSymbols");
  addKey("KEY_kernelreference");
  addKey("KEY_dataSize");
  addKey("KEY_startup");
  addKey("KEY_run");
  addKey("KEY_ram");
}

