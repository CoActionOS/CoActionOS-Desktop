#ifndef CKERNELINSTALLER_H
#define CKERNELINSTALLER_H

#include <QWidget>
#include "CLinkPtr.h"

namespace Ui {
  class CKernelInstaller;
}

class CKernelInstaller : public QWidget, public CLinkPtr
{
  Q_OBJECT
  
public:
  explicit CKernelInstaller(QWidget *parent = 0);
  ~CKernelInstaller();

  void setLink(CLink *d);
  void savePreferences();
  void loadPreferences();
  
signals:
  void aboutToReset(void);
  void projectUpdated(QString workspace, QString project, QString conf);
  void installApps(void);

public slots:
  void setProject(QString project);
  void setConfiguration(QString conf);
  void installKernel(void);

private slots:
  void connected(bool);
  void addProject(QString name);
  void projectSelected(QString name);

  void on_installButton_clicked();
  void on_verifyInstallCheckBox_clicked(bool checked);

  void on_uploadButton_clicked();

private:
  Ui::CKernelInstaller *ui;

  void resizeEvent(QResizeEvent * event);

};

#endif // CKERNELINSTALLER_H
