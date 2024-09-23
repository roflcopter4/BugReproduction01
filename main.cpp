#include "Common.h"
#include "MainWindow.h"
#include "util.h"

#include <QApplication>

int main(int argc, char *argv[])
{
#ifdef Q_OS_WINDOWS
    util::OpenConsoleWindow();
#endif
    QApplication app(argc, argv);
    MainWindow   mw;
    mw.show();
    return QApplication::exec();
}
