#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(ChatGpt_LIB)
#  define ChatGpt_EXPORT Q_DECL_EXPORT
# else
#  define ChatGpt_EXPORT Q_DECL_IMPORT
# endif
#else
# define ChatGpt_EXPORT
#endif
