#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(TONGYIAI_LIB)
#  define TONGYIAI_EXPORT Q_DECL_EXPORT
# else
#  define TONGYIAI_EXPORT Q_DECL_IMPORT
# endif
#else
# define TONGYIAI_EXPORT
#endif
