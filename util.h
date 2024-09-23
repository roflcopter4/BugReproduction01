#pragma once

#include "Common.h"
#include <QString>

namespace util {

extern ND QString GetErrorMessage(unsigned errVal);

#ifdef Q_OS_WIN32
extern void OpenConsoleWindow();
#endif

} // namespace util
