#include "mainwindow.h"
#include <QApplication>
#include <globals.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    g_MainWindow = &w;
    w.show();

    return a.exec();
}
