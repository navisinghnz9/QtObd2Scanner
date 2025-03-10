#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Settings/SettingsWidget.h"

#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>
#include <QPoint>
#include <QListWidgetItem>
#include <QHash>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QString extractDataInsideParentheses(const QString &text);
    QString extractTextOutsideParentheses(const QString &text);

private slots:
    void OnSettingsMenuClicked();
    void onSettingsChanged(QString port, int baud);

    void startScanning();
    void addDeviceToList(const QBluetoothDeviceInfo &device);
    void scanFinished();

    void showBTOptionsMenu(const QPoint &pos);
    void infoDevice(QListWidgetItem *item);
    void pairDevice(QListWidgetItem *item);
    void connectDevice(QListWidgetItem *item);

private:
    Ui::MainWindow *ui;
    SettingsWidget settingsWidget;

    QString m_port;
    int m_baud;

    QBluetoothDeviceDiscoveryAgent *discoveryAgent;
    QHash<QString, QBluetoothDeviceInfo> deviceHashTable;
};
#endif // MAINWINDOW_H
