#ifndef CCOMPRESS_H
#define CCOMPRESS_H

#include "CSdk.h"
#include <QString>

class CCompress : public CSdk
{
public:
  CCompress();

  int compress(QString dest, QString src);
  int decompress(QString dest, QString src);
};

#endif // CCOMPRESS_H
