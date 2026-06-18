QT += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = AutoParts
TEMPLATE = app

SOURCES += \
    src/Utils/database.cpp \
    src/main.cpp \
    src/View/mainwindow.cpp

HEADERS += \
    src/Utils/database.h \
    src/View/mainwindow.h

FORMS += \
    src/View/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
