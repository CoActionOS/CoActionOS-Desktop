#ifndef CAPPLICATIONINSTALLER_H
#define CAPPLICATIONINSTALLER_H

#include <QWidget>
#include <QString>
#include <QStringList>

#include "CLinkPtr.h"

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

signals:
  void runApplication(QString path);
  int isRunning(QString);

public slots:
  void installApplication(void);
  
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

private:
  int install(void);
  int installApp();
  QString projectRunPath(void);



  Ui::CApplicationInstaller *ui;
};

#endif // CAPPLICATIONINSTALLER_H
