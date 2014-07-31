#ifndef CSETTINGS_H
#define CSETTINGS_H

#include <QStringList>
#include <QSettings>
#include <QCoreApplication>

class CSettings
{
public:
  CSettings(QString name, bool create = true, QSettings::Format format = QSettings::IniFormat);
  CSettings(QSettings::Scope scope, QString app = QCoreApplication::applicationName(), QString org = "CoActionOS, Inc", QSettings::Format format = QSettings::IniFormat);
  ~CSettings();

  static QString app(){ return "CoActionOS-QtSDK"; }
  static QSettings::Scope userScope(){ return QSettings::UserScope; }


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
