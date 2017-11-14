#include "mainwindow.h"
#include "RunGuard.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    RunGuard guard("PitM_Memory");
    if (!guard.tryToRun()) {
        return 0;
    }

    QApplication a(argc, argv);

    QApplication::setQuitOnLastWindowClosed(false);
    memory::MainWindow w;
    w.show();

    return a.exec();
}
