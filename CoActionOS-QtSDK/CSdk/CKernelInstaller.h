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
  
signals:
  void aboutToReset(void);

private slots:
  void connected(bool);
  void addProject(QString name);
  void projectSelected(QString name);

  void on_installButton_clicked();
  void on_verifyInstallCheckBox_clicked(bool checked);

private:
  Ui::CKernelInstaller *ui;

};

#endif // CKERNELINSTALLER_H
