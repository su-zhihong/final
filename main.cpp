#include "mainwindow.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setApplicationName(u8"智能家居控制中心");

    MainWindow w;
    w.show();

    return a.exec();
}
