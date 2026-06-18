QT += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = AutoParts
TEMPLATE = app

SOURCES += \
    src/Controller/authcontroller.cpp \
    src/Controller/suppliercontroller.cpp \
    src/Controller/usercontroller.cpp \
    src/Model/accountingview.cpp \
    src/Model/detail.cpp \
    src/Model/pricechange.cpp \
    src/Model/pricehistoryview.cpp \
    src/Model/supplier.cpp \
    src/Model/supplierpricechange.cpp \
    src/Model/supply.cpp \
    src/Model/supplyview.cpp \
    src/Model/user.cpp \
    src/Model/userinfoview.cpp \
    src/Utils/database.cpp \
    src/Utils/sessionmanager.cpp \
    src/View/logindialog.cpp \
    src/View/registerdialog.cpp \
    src/View/supplierdialog.cpp \
    src/View/userdialog.cpp \
    src/main.cpp \
    src/View/mainwindow.cpp

HEADERS += \
    src/Controller/authcontroller.h \
    src/Controller/suppliercontroller.h \
    src/Controller/usercontroller.h \
    src/Model/accountingview.h \
    src/Model/detail.h \
    src/Model/pricechange.h \
    src/Model/pricehistoryview.h \
    src/Model/supplier.h \
    src/Model/supplierpricechange.h \
    src/Model/supply.h \
    src/Model/supplyview.h \
    src/Model/user.h \
    src/Model/userinfoview.h \
    src/Utils/database.h \
    src/Utils/sessionmanager.h \
    src/View/logindialog.h \
    src/View/mainwindow.h \
    src/View/registerdialog.h \
    src/View/supplierdialog.h \
    src/View/userdialog.h

FORMS += \
    src/View/logindialog.ui \
    src/View/mainwindow.ui \
    src/View/registerdialog.ui \
    src/View/supplierdialog.ui \
    src/View/userdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target