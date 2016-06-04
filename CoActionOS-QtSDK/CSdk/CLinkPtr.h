#ifndef CLINKPTR_H
#define CLINKPTR_H

#include <QString>

#include "CSdk.h"
#include "CLink.h"

class CLinkPtr : public CSdk
{
public:
  CLinkPtr() : link_(0) {}
  virtual void setLink(CLink * l){ link_ = l; }
  CLink * link(){ return link_; }
  QString errorMessage(void){return QString::fromStdString(link()->error_message()); }

private:
  CLink * link_;

};

#endif // CLINKPTR_H
