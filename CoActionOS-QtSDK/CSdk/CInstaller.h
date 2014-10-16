#ifndef CINSTALLER_H
#define CINSTALLER_H

#include <QWidget>
#include <QString>

#include "CSdk.h"

namespace Ui {
  class CInstaller;
}

class CInstaller : public QWidget, public CSdk
{
  Q_OBJECT
public:
  explicit CInstaller(QWidget *parent = 0);
  ~CInstaller();

  void setWorkspace(QString workspace);
  QString workspace(void);
  
  void setSettings(QString settings){ this->settings = settings; }
  void setPrefix(QString prefix){ this->settingsPrefix = "KEY_" + prefix; }

  void updateProjectList(void);
  QString project();
  QString configuration();
  QString projectPath(){ return workspace() + "/" + project(); }
  QString configurationPath(){ return projectPath() + "/" + configuration(); }

  void showLabels(bool v = true);

  int count() const;
  void setCurrentProject(int index);

  enum {
      INVALID_SIGNATURE = 0xFFFFFFFF
  };

  void selectProject(QString project);
  void selectConfiguration(QString conf);

  void savePreferences();
  void loadPreferences();

signals:
  QStringList addConfiguration(QStringList);
  void clearConfigurations(void);
  void addProject(QString);
  void projectSelected(QString);
  void projectUpdated(QString workspace, QString project, QString conf);




private:
  Ui::CInstaller *ui;


  QString addAlias(QString symbol, bool useFloat);
  QString settings;
  QString settingsPrefix;
  void addConfigurations(QStringList list);

  bool updateConfiguration;

private slots:
  void workspaceUpdated();

  void on_addConfigurationButton_clicked();
  void on_addProjectButton_clicked();
  void on_refreshWorkspaceButton_clicked();
  void on_clearConfigurationsButton_clicked();
  void on_projectNameComboBox_currentIndexChanged(const QString &arg1);
  void on_configurationComboBox_currentIndexChanged(const QString &arg1);

  void emitUpdated();

};

#endif // CINSTALLER_H
