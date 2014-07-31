#include "CSettings.h"

#include <QFileInfo>

CSettings::CSettings(QString name, bool create, QSettings::Format format){
  QFileInfo info(name);
  QFileInfo dir(info.path());
  settings = 0;
  if( dir.exists() == false ){
      return;
    }

  if( dir.fileName() == QString() ){
      return;
    }

  if( (create == true) && !info.exists() ){
      qDebug("Create name %s %s dir %s", name.toLocal8Bit().constData(),
             info.fileName().toLocal8Bit().constData(),
             dir.fileName().toLocal8Bit().constData());
  }

  if( (create == true) || (info.exists()) ){
      settings = new QSettings(name, format);
    }
}

CSettings::CSettings(QSettings::Scope scope, QString app, QString org, QSettings::Format format){
  settings = new QSettings(format, scope, org, app);
}

int CSettings::getKeyIndex(QString keyname){
  int i;
  for(i=0; i < keys.count(); i++){
      qDebug("Compare %s to %s", keyname.toLocal8Bit().constData(),
             keys.at(i).toLocal8Bit().constData());
      if( keyname == keys.at(i) ){
          return i;
        }
    }
  return -1;
}

CSettings::~CSettings(){
  if( settings != 0 ){
      delete settings;
    }
}

bool CSettings::exists(void){
  if( settings == 0 ){
      return false;
    }

  return true;
}

void CSettings::addKey(QString key){
  keys.append(key);
}

QString CSettings::getStringKey(int key){
  if( settings == 0 ){
      return QString();
    }
  return settings->value(keys.at(key)).toString();
}

QString CSettings::getStringKey(QString key){
  if( settings == 0 ){
      return QString();
    }
  return settings->value(key).toString();
}


QStringList CSettings::getStringListKey(int key){
  if( settings == 0 ){
      return QStringList();
    }
  return settings->value(keys.at(key)).toStringList();
}

QStringList CSettings::getStringListKey(QString key){
  if( settings == 0 ){
      return QStringList();
    }
  return settings->value(key).toStringList();
}

bool CSettings::getBoolKey(int key){
  if( settings == 0 ){
      return false;
    }
  return settings->value(keys.at(key)).toBool();
}

bool CSettings::getBoolKey(QString key){
  if( settings == 0 ){
      return false;
    }
  return settings->value(key).toBool();
}

void CSettings::setKey(int key, QString value){
  if( settings != 0 ){
      settings->setValue(keys.at(key), value);
    }
}

void CSettings::setKey(QString key, QString value){
  if( settings != 0 ){
      settings->setValue(key, value);
    }
}

void CSettings::setKey(int key, QStringList value){
  if( settings != 0 ){
      settings->setValue(keys.at(key), value);
    }
}

void CSettings::setKey(QString key, QStringList value){
  if( settings != 0 ){
      settings->setValue(key, value);
    }
}


void CSettings::setKey(int key, bool value){
  if( settings != 0 ){
      settings->setValue(keys.at(key), value);
    }
}

void CSettings::setKey(QString key, bool value){
  if( settings != 0 ){
      settings->setValue(key, value);
    }
}
