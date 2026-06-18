QT += core gui widgets network
CONFIG += c++17
TARGET = AutoPartsClient
TEMPLATE = app
OBJECTS_DIR = obj_client

SOURCES += \
    main.cpp \
    View/logindialog.cpp \
    View/mainwindow.cpp \
    View/registerdialog.cpp \
    View/supplierdialog.cpp \
    View/userdialog.cpp \
    TcpClient/tcpclient.cpp \
    Utils/sessionmanager.cpp

HEADERS += \
    View/logindialog.h \
    View/mainwindow.h \
    View/registerdialog.h \
    View/supplierdialog.h \
    View/userdialog.h \
    TcpClient/tcpclient.h \
    Utils/sessionmanager.h

FORMS += \
    View/logindialog.ui \
    View/mainwindow.ui \
    View/registerdialog.ui \
    View/supplierdialog.ui \
    View/userdialog.ui

INCLUDEPATH += . View TcpClient Utils