#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QSettings>

#include <QBluetoothLocalDevice>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSettings settings("IFS", "obdqmltool");
    settings.beginGroup("settings");
    m_port = settings.value("comport", "").toString();
    m_baud = settings.value("baudrate", 0).toInt();
    settings.endGroup();
    ui->status_comPortLabel->setText("Com port: " + m_port);
    ui->status_comBaudLabel->setText("Baud Rate: " + QString::number(m_baud));

    connect(ui->actionSettings, SIGNAL(triggered()), this, SLOT(OnSettingsMenuClicked()));
    connect(&settingsWidget, &SettingsWidget::saveSettings, this, &MainWindow::onSettingsChanged);

    discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);

    // Connect signals and slots
    connect(ui->btnBTScanDevs, &QPushButton::clicked, this, &MainWindow::startScanning);
    connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &MainWindow::addDeviceToList);
    connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished, this, &MainWindow::scanFinished);

    ui->lstBTDevs->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->lstBTDevs, &QListWidget::customContextMenuRequested, this, &MainWindow::showBTOptionsMenu);
}

void MainWindow::startScanning()
{
    qDebug() <<"Scanning devices...";
    ui->lstBTDevs->clear();
    deviceHashTable.clear();
    ui->btnBTScanDevs->setEnabled(false);
    discoveryAgent->start();
}

void MainWindow::addDeviceToList(const QBluetoothDeviceInfo &device)
{
    QString address = device.address().toString();

    // Only add device if it's not already in the hash table
    if (!deviceHashTable.contains(address))
    {
        // Add the device info to the hash table
        deviceHashTable.insert(address, device);

        // Add device name and address to the list widget
        QString deviceInfo = device.name() + " (" + address + ")";
        ui->lstBTDevs->addItem(deviceInfo);
    }
}

void MainWindow::scanFinished()
{
    ui->btnBTScanDevs->setEnabled(true);
}

void MainWindow::showBTOptionsMenu(const QPoint &pos)
{
    // Get the selected item at the cursor position
    QListWidgetItem *item = ui->lstBTDevs->itemAt(pos);
    if (item)
    {
        // Create the context menu
        QMenu contextMenu(this);

        // Create actions
        QAction *infoAction = new QAction("Info", this);
        QAction *pairAction = new QAction("Pair", this);
        QAction *connectAction = new QAction("Connect", this);

        // Connect actions to slots
        connect(infoAction, &QAction::triggered, this, [this, item]() { infoDevice(item); });
        connect(pairAction, &QAction::triggered, this, [this, item]() { pairDevice(item); });
        connect(connectAction, &QAction::triggered, this, [this, item]() { connectDevice(item); });

        // Add actions to the menu
        contextMenu.addAction(infoAction);
        contextMenu.addAction(pairAction);
        contextMenu.addAction(connectAction);

        // Show the context menu
        contextMenu.exec(ui->lstBTDevs->viewport()->mapToGlobal(pos));
    }
}

QString MainWindow::extractDataInsideParentheses(const QString &text)
{
    // Define the regular expression to match data inside parentheses
    QRegularExpression regex(R"(\(([^)]+)\))");
    QRegularExpressionMatch match = regex.match(text);

    if (match.hasMatch()) {
        // Extract the data inside parentheses
        return match.captured(1);  // captured(1) is the first group, which is the content inside parentheses
    }

    return QString();  // Return an empty string if no match is found
}

QString MainWindow::extractTextOutsideParentheses(const QString &text)
{
    QRegularExpression regex(R"((.*?)\s*\([^)]*\))");

    QRegularExpressionMatch match = regex.match(text);

    if (match.hasMatch()) {
        QString result = match.captured(1).trimmed();  // trim the whitespace at the end
        return result;
    }

    return QString();  // Return an empty string if no match is found
}

void MainWindow::infoDevice(QListWidgetItem *item)
{
    qDebug() << "Item selected: " << item->text();
    QString address = extractDataInsideParentheses(item->text());
    QBluetoothDeviceInfo device = deviceHashTable.value(address);

    // Implement pairing logic here
    // For now, we just show a message
    qDebug() << "Pairing with device: " << device.name();
    // Add the actual Bluetooth pairing logic here

    qDebug() << "Connecting to device:";
    qDebug() << "Name: " << device.name();
    qDebug() << "Address: " << device.address().toString();
    qDebug() << "Major Device Class: " << device.majorDeviceClass();
    qDebug() << "RSSI: " << device.rssi();
    qDebug() << "Core Configuration: " << device.coreConfigurations();
}

void MainWindow::pairDevice(QListWidgetItem *item)
{
    QString address = extractDataInsideParentheses(item->text());
    QBluetoothDeviceInfo device = deviceHashTable.value(address);

    qDebug() << "Pairing with device: " << device.name();
}

// Handle connection to the selected device
void MainWindow::connectDevice(QListWidgetItem *item)
{
    QString address = extractDataInsideParentheses(item->text());
    QBluetoothDeviceInfo device = deviceHashTable.value(address);

    qDebug() << "Connecting with device: " << device.name();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::OnSettingsMenuClicked()
{
    settingsWidget.setSettings(m_port, m_baud);
    settingsWidget.show();
}

void MainWindow::onSettingsChanged(QString port, int baud)
{
    qDebug() <<QString("Saving new settings: port=%1, baudrate=%2").arg(port).arg(baud);

    // saving the changed settings
    QSettings settings("IFS", "obdqmltool");
    settings.beginGroup("settings");
    settings.setValue("comport", port);
    settings.setValue("baudrate", baud);
    settings.endGroup();

    // updating class memebrs with connection settings
    m_port = port;
    m_baud = baud;

    // showing current port and baudrate in UI labels
    ui->status_comPortLabel->setText("Com port: " + m_port);
    ui->status_comBaudLabel->setText("Baud Rate: " + QString::number(m_baud));
}
