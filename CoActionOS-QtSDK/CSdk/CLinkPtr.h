#ifndef CLINKPTR_H
#define CLINKPTR_H

#include <QString>
#include "CLink.h"

class CLinkPtr
{
public:
  CLinkPtr() : link_(0) {};
  virtual void setLink(CLink * l){ link_ = l; }
  CLink * link(){ return link_; }
  QString errorMessage(void){return QString::fromStdString(link()->errorMessage()); }

private:
  CLink * link_;

};

#endif // CLINKPTR_H
