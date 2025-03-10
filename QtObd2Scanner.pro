QT       += core gui bluetooth

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Bluetooth/BluetoothClient.cpp \
    Settings/SettingsWidget.cpp \
    main.cpp \
    MainWindow.cpp

HEADERS += \
    Bluetooth/BluetoothClient.h \
    MainWindow.h \
    Settings/SettingsWidget.h

FORMS += \
    MainWindow.ui \
    SettingsWidget.ui \
    TroubleCodes.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
