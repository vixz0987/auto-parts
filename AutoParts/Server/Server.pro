QT += core sql network
CONFIG += c++17 console
TARGET = AutoPartsServer
TEMPLATE = app
OBJECTS_DIR = obj_server

SOURCES += \
    main.cpp \
    Model/accountingview.cpp \
    Model/detail.cpp \
    Model/pricechange.cpp \
    Model/pricehistoryview.cpp \
    Model/supplier.cpp \
    Model/supplierpricechange.cpp \
    Model/supply.cpp \
    Model/supplyview.cpp \
    Model/user.cpp \
    Model/userinfoview.cpp \
    Controller/authcontroller.cpp \
    Controller/suppliercontroller.cpp \
    Controller/usercontroller.cpp \
    Utils/database.cpp \
    TcpServer/tcpserver.cpp

HEADERS += \
    Model/accountingview.h \
    Model/detail.h \
    Model/pricechange.h \
    Model/pricehistoryview.h \
    Model/supplier.h \
    Model/supplierpricechange.h \
    Model/supply.h \
    Model/supplyview.h \
    Model/user.h \
    Model/userinfoview.h \
    Controller/authcontroller.h \
    Controller/suppliercontroller.h \
    Controller/usercontroller.h \
    Utils/database.h \
    TcpServer/tcpserver.h

INCLUDEPATH += . Model Controller Utils TcpServer