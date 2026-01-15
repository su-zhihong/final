#include "mainwindow.h"
#include <QApplication>

// Qt6 移除了QTextCodec，这里用utf8_source编译选项+原生字符串，彻底解决中文乱码
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // 全局应用程序名称，中文正常显示
    a.setApplicationName(u8"智能家居控制中心");

    MainWindow w;
    w.show();

    return a.exec();
}
