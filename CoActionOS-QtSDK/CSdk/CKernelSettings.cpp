#include "CKernelSettings.h"

CKernelSettings::CKernelSettings(QString projectPath, bool create) :
  CSettings(projectPath + "/ckernelsettings.ini", create)
{
  addKey("KEY_configurations");
  addKey("KEY_configuration");
  addKey("KEY_verifyinstall");
  addKey("KEY_floatsymbols");
  addKey("KEY_startaddr");
  addKey("KEY_downloadsymbols");
}
