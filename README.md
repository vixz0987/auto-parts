# course-project

## структура приложения 

```
AutoParts/
├── .gitignore
├── AutoParts.pro
├── Client/
│   ├── Client.pro
│   ├── main.cpp
│   ├── Services/
│   │   ├── clientservice.cpp
│   │   ├── clientservice.h
│   │   ├── sessionmanager.cpp
│   │   └── sessionmanager.h
│   ├── TcpClient/
│   │   ├── tcpclient.cpp
│   │   └── tcpclient.h
│   └── View/
│       ├── detaildialog.cpp
│       ├── detaildialog.h
│       ├── detaildialog.ui
│       ├── logindialog.cpp
│       ├── logindialog.h
│       ├── logindialog.ui
│       ├── mainwindow.cpp
│       ├── mainwindow.h
│       ├── mainwindow.ui
│       ├── pricechangedialog.cpp
│       ├── pricechangedialog.h
│       ├── pricechangedialog.ui
│       ├── profiledialog.cpp
│       ├── profiledialog.h
│       ├── profiledialog.ui
│       ├── registerdialog.cpp
│       ├── registerdialog.h
│       ├── registerdialog.ui
│       ├── supplierdialog.cpp
│       ├── supplierdialog.h
│       ├── supplierdialog.ui
│       ├── supplydialog.cpp
│       ├── supplydialog.h
│       ├── supplydialog.ui
│       ├── userdialog.cpp
│       ├── userdialog.h
│       └── userdialog.ui
└── Server/
    ├── Controller/
    │   ├── authcontroller.cpp
    │   ├── authcontroller.h
    │   ├── detailcontroller.cpp
    │   ├── detailcontroller.h
    │   ├── pricechangecontroller.cpp
    │   ├── pricechangecontroller.h
    │   ├── suppliercontroller.cpp
    │   ├── suppliercontroller.h
    │   ├── supplycontroller.cpp
    │   ├── supplycontroller.h
    │   ├── usercontroller.cpp
    │   └── usercontroller.h
    ├── main.cpp
    ├── Model/
    │   ├── accountingview.cpp
    │   ├── accountingview.h
    │   ├── currentpriceview.cpp
    │   ├── currentpriceview.h
    │   ├── detail.cpp
    │   ├── detail.h
    │   ├── pricechange.cpp
    │   ├── pricechange.h
    │   ├── pricehistoryview.cpp
    │   ├── pricehistoryview.h
    │   ├── supplier.cpp
    │   ├── supplier.h
    │   ├── supply.cpp
    │   ├── supply.h
    │   ├── supplyview.cpp
    │   ├── supplyview.h
    │   ├── user.cpp
    │   ├── user.h
    │   ├── userinfoview.cpp
    │   └── userinfoview.h
    ├── Server.pro
    ├── TcpServer/
    │   ├── tcpserver.cpp
    │   └── tcpserver.h
    └── Utils/
        ├── database.cpp
        └── database.h
```
