# course-project

## структура приложения 

```
AutoParts/
├── AutoParts.pro
├── .gitignore
│
├── Server/                # Сервер (консольное приложение)
│   ├── Server.pro
│   ├── main.cpp
│   ├── Controller/
│   │   ├── authcontroller.h
│   │   ├── authcontroller.cpp
│   │   ├── suppliercontroller.h
│   │   ├── suppliercontroller.cpp
│   │   ├── usercontroller.h
│   │   └── usercontroller.cpp
│   ├── Model/
│   │   ├── user.h
│   │   ├── user.cpp
│   │   ├── userinfoview.h
│   │   ├── userinfoview.cpp
│   │   ├── supplier.h
│   │   ├── supplier.cpp
│   │   ├── detail.h
│   │   ├── detail.cpp
│   │   ├── supply.h
│   │   ├── supply.cpp
│   │   ├── supplyview.h
│   │   ├── supplyview.cpp
│   │   ├── pricechange.h
│   │   ├── pricechange.cpp
│   │   ├── pricehistoryview.h
│   │   ├── pricehistoryview.cpp
│   │   ├── accountingview.h
│   │   ├── accountingview.cpp
│   │   ├── supplierpricechange.h
│   │   └── supplierpricechange.cpp
│   ├── TcpServer/
│   │   ├── tcpserver.h
│   │   └── tcpserver.cpp
│   └── Utils/
│       ├── database.h
│       └── database.cpp
│
└── Client/                      # Клиент (GUI, без доступа к БД)
    ├── Client.pro
    ├── main.cpp
    ├── TcpClient/
    │   ├── tcpclient.h
    │   └── tcpclient.cpp
    ├── Utils/
    │   ├── sessionmanager.h
    │   └── sessionmanager.cpp
    └── View/
        ├── mainwindow.h
        ├── mainwindow.cpp
        ├── mainwindow.ui
        ├── logindialog.h
        ├── logindialog.cpp
        ├── logindialog.ui
        ├── registerdialog.h
        ├── registerdialog.cpp
        ├── registerdialog.ui
        ├── userdialog.h
        ├── userdialog.cpp
        ├── userdialog.ui
        ├── supplierdialog.h
        ├── supplierdialog.cpp
        └── supplierdialog.ui
```
