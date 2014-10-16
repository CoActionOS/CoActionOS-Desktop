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
  QString stringKey(int a){ return getStringKey(a); }
  QString getStringKey(QString);
  QString stringKey(QString a){ return getStringKey(a); }
  QStringList getStringListKey(int);
  QStringList stringListKey(int a){ return getStringListKey(a); }
  QStringList getStringListKey(QString);
  QStringList stringListKey(QString a){ return getStringListKey(a); }
  bool getBoolKey(int a);
  bool boolKey(int a){ return getBoolKey(a); }
  bool getBoolKey(QString);
  bool boolKey(QString a){ return getBoolKey(a); }
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
