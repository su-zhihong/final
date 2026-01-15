QT += core gui widgets sql network charts printsupport

CONFIG += c++17
DEFINES += QT_DEPRECATED_WARNINGS

# 目标生成名称
TARGET = SmartHomeControlCenter
TEMPLATE = app

# 源文件列表
SOURCES += main.cpp\
        mainwindow.cpp \
        devicemodel.cpp \
        datathread.cpp \
        databasehelper.cpp

# 头文件列表
HEADERS  += mainwindow.h \
        devicemodel.h \
        datathread.h \
        databasehelper.h

# Qt6中文乱码全局解决方案（核心！）
CONFIG += utf8_source
