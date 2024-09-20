#include "MainWindow.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    auto *app = new QApplication(argc, argv);
    auto *mw  = new MainWindow();

    qDebug() << u"Showing main window."_s;
    mw->show();
    int ret = QApplication::exec();

    delete mw;
    delete app;
    return ret;
}
