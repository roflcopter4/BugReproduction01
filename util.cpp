#include "Common.h"

#include <QApplication>
#include <QString>
#include <iostream>
#include <mutex>

#ifdef Q_OS_WIN32
# ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
# endif
# include <Windows.h>
#endif

/****************************************************************************************/

namespace util {

ND QString GetErrorMessage(unsigned errVal)
{
#if defined Q_OS_WIN
    wchar_t buf[512];
    DWORD res = ::FormatMessageW(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, errVal, LANG_SYSTEM_DEFAULT, buf, std::size(buf), nullptr
    );
    return (res < 2 ? u"Unknown error"_s : QString::fromWCharArray(buf, res - 2)) +
           u" (" + QString::number(errVal, 16).toUpper() + u')';
#else
    char  buf[512]{};
    char *msg = strerror_r(errVal, buf, std::size(buf));
    return QString::fromUtf8(msg) + QSV(" (") + QString::number(errVal) + u')';
#endif
}

#ifdef Q_OS_WIN32

static void do_OpenConsoleWindow()
{
    ::FreeConsole();
    if (!::AllocConsole()) {
        DWORD err = ::GetLastError();
        WCHAR buf[128];
        swprintf_s(buf, std::size(buf),
                   L"Failed to allocate a console (error %ls). "
                   L"If this happens your computer is probably on fire.",
                   reinterpret_cast<wchar_t const *>(GetErrorMessage(err).data()));
        ::MessageBoxW(nullptr, buf, L"Fatal Error", MB_OK | MB_ICONERROR);
#ifdef QT_VERSION
        QApplication::exit(1);
#else
        ::exit(1);
#endif
    }

    int ret;
    ::FILE *conout;
    ret = ::_wfreopen_s(&conout, L"CONOUT$", L"wt", stdout);
    assert(ret == 0);
    ret = ::_wfreopen_s(&conout, L"CONOUT$", L"wt", stderr);
    assert(ret == 0);
    ret = ::_wfreopen_s(&conout, L"CONIN$", L"rt", stdin);
    assert(ret == 0);
}

void OpenConsoleWindow()
{
    static std::once_flag flag;
    std::call_once(flag, do_OpenConsoleWindow);
}

#endif

} // namespace util
