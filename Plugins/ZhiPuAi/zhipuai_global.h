#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(ZHIPUAI_LIB)
#  define ZHIPUAI_EXPORT Q_DECL_EXPORT
# else
#  define ZHIPUAI_EXPORT Q_DECL_IMPORT
# endif
#else
# define ZHIPUAI_EXPORT
#endif
