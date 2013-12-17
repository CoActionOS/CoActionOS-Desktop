#ifndef CSETTINGS_H
#define CSETTINGS_H

#include <QStringList>
#include <QSettings>

class CSettings
{
public:
  CSettings(QString name, bool create = true, QSettings::Format format = QSettings::IniFormat);
  CSettings(QSettings::Scope scope, QString app, QString org = "CoActionOS, Inc", QSettings::Format format = QSettings::IniFormat);
  ~CSettings();


  bool exists(void);
  int getKeyIndex(QString keyname);

  void addKey(QString);
  QString getStringKey(int);
  QString getStringKey(QString);
  QStringList getStringListKey(int);
  QStringList getStringListKey(QString);
  bool getBoolKey(int);
  bool getBoolKey(QString);
  void setKey(int, QStringList);
  void setKey(QString, QStringList);
  void setKey(int, QString);
  void setKey(QString, QString);
  void setKey(int, bool);
  void setKey(QString, bool);

private:
  QStringList keys;
  QSettings * settings;
};

#endif // CSETTINGS_H
