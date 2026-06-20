QT += core gui widgets network
CONFIG += c++17
TARGET = AutoPartsClient
TEMPLATE = app
OBJECTS_DIR = obj_client

SOURCES += \
    View/detaildialog.cpp \
    View/pricechangedialog.cpp \
    View/profiledialog.cpp \
    View/supplydialog.cpp \
    main.cpp \
    View/logindialog.cpp \
    View/mainwindow.cpp \
    View/registerdialog.cpp \
    View/supplierdialog.cpp \
    View/userdialog.cpp \
    TcpClient/tcpclient.cpp \
    Utils/sessionmanager.cpp

HEADERS += \
    Utils/datatypes.h \
    View/detaildialog.h \
    View/logindialog.h \
    View/mainwindow.h \
    View/pricechangedialog.h \
    View/profiledialog.h \
    View/registerdialog.h \
    View/supplierdialog.h \
    View/supplydialog.h \
    View/userdialog.h \
    TcpClient/tcpclient.h \
    Utils/sessionmanager.h

FORMS += \
    View/detaildialog.ui \
    View/logindialog.ui \
    View/mainwindow.ui \
    View/pricechangedialog.ui \
    View/profiledialog.ui \
    View/registerdialog.ui \
    View/supplierdialog.ui \
    View/supplydialog.ui \
    View/userdialog.ui

INCLUDEPATH += . View TcpClient Utils