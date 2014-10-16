#ifndef CAPPLICATIONINSTALLER_H
#define CAPPLICATIONINSTALLER_H

#include <QWidget>
#include <QString>
#include <QStringList>

#include "CLinkPtr.h"
#include "CSdk.h"

namespace Ui {
  class CApplicationInstaller;
}

class CApplicationInstaller : public QWidget, public CLinkPtr
{
  Q_OBJECT
  
public:
  explicit CApplicationInstaller(QWidget *parent = 0);
  ~CApplicationInstaller();

  void setLink(CLink * d);

  void installAll(void);

signals:
  void runApplication(QString path);
  int isRunning(QString);
  void selected(QString path);
  void projectUpdated(QString workspace, QString project, QString conf);

public slots:
  void installApplication(void);

  void setProject(QString project);
  void setConfiguration(QString conf);
  
private slots:
  void connected(bool);
  void addProject(QString name);
  void projectSelected(QString name);
  void installPathUpdated(void);
  void on_installButton_clicked();
  void on_uninstallButton_clicked();
  void on_optionsStartupCheckBox_clicked(bool checked);
  void on_optionsRamCheckBox_clicked(bool checked);
  void on_optionsRunCheckBox_clicked(bool checked);
  void on_dataSizeSpinBox_valueChanged(int arg1);
  void on_filesInstallButton_clicked();

  void uninstallPrompt(int v);
  void installPrompt(int v);

private:
  void resizeEvent(QResizeEvent * event);
  int install(void);
  int installApp();
  int uninstallApp();
  QString projectRunPath(void);

  bool parseDataSize(void);


  int dataSize;
  int bssSize;

  bool copyDirectory(QString subdir);


  Ui::CApplicationInstaller *ui;
};

#endif // CAPPLICATIONINSTALLER_H
