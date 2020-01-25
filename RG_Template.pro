QT += core gui
QT += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RG_Template
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    widgetopengldraw.cpp

HEADERS  += mainwindow.h \
    widgetopengldraw.h

FORMS    += mainwindow.ui

QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += -std=c++0x
CONFIG += c++11

#INCLUDEPATH += pot do GLM, ne pozabi spremeniti
INCLUDEPATH += C:/Faks/RG/glm-0.9.9.6

DEFINES+= "TEX_PATH=\"\\\"$$PWD/\\\"\""

LIBS += -lopengl32
