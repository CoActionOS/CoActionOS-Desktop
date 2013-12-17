#ifndef CSDK_GLOBAL_H
#define CSDK_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(CSDK_LIBRARY)
#  define CSDKSHARED_EXPORT Q_DECL_EXPORT
#else
#  define CSDKSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // CSDK_GLOBAL_H
