QT += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = AutoParts
TEMPLATE = app

SOURCES += \
    src/Controller/authcontroller.cpp \
    src/Model/user.cpp \
    src/Utils/database.cpp \
    src/Utils/sessionmanager.cpp \
    src/View/logindialog.cpp \
    src/View/registerdialog.cpp \
    src/main.cpp \
    src/View/mainwindow.cpp

HEADERS += \
    src/Controller/authcontroller.h \
    src/Model/user.h \
    src/Utils/database.h \
    src/Utils/sessionmanager.h \
    src/View/logindialog.h \
    src/View/mainwindow.h \
    src/View/registerdialog.h

FORMS += \
    src/View/logindialog.ui \
    src/View/mainwindow.ui \
    src/View/registerdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target