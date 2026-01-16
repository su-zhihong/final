QT += core gui widgets network

CONFIG += c++17
DEFINES += QT_DEPRECATED_WARNINGS
CONFIG += utf8_source

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

TARGET = MockGateway
TEMPLATE = app
