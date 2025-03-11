#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QSettings>
#include <QFileDialog>
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

    initializeCanbusUI();
}


void MainWindow::initializeCanbusUI()
{

    // by default we will have display style 2
    m_canDispStyle = 2;

    m_demoMode = false;

    // adding sorting optiions for Canbus data
    ui->canStyleComboBox->addItem("Sort by ParamID");
    ui->canStyleComboBox->addItem("Sort by Source");
    ui->canStyleComboBox->addItem("Sort by Param, Overwrite and highlight changes");
    ui->canStyleComboBox->setCurrentIndex(2);

    if (m_canDispStyle == 0)
    {
        ui->canMsgTableWidget->setRowCount(1);
    }
    else if (m_canDispStyle == 1)
    {
        ui->canMsgTableWidget->setRowCount(1);
    }
    else if (m_canDispStyle == 2)
    {
        ui->canMsgTableWidget->setColumnCount(8);
        ui->canMsgTableWidget->setHorizontalHeaderItem(0,new QTableWidgetItem("CANID"));
        ui->canMsgTableWidget->setHorizontalHeaderItem(1,new QTableWidgetItem("SOURCE"));
        ui->canMsgTableWidget->setHorizontalHeaderItem(2,new QTableWidgetItem("BYTE 0"));
        ui->canMsgTableWidget->setHorizontalHeaderItem(3,new QTableWidgetItem("BYTE 1"));
        ui->canMsgTableWidget->setHorizontalHeaderItem(4,new QTableWidgetItem("BYTE 2"));
        ui->canMsgTableWidget->setHorizontalHeaderItem(5,new QTableWidgetItem("BYTE 3"));
        ui->canMsgTableWidget->setHorizontalHeaderItem(6,new QTableWidgetItem("BYTE 4"));
        ui->canMsgTableWidget->setHorizontalHeaderItem(7,new QTableWidgetItem("BYTE 5"));
    }

    // adding protocols for CANbus protocols
    ui->canProtocolComboBox->addItem("11:J1850 PWM");
    ui->canProtocolComboBox->addItem("12:J1850 VPW");
    ui->canProtocolComboBox->addItem("21:ISO 9141 (no header no autoinit)");
    ui->canProtocolComboBox->addItem("22:ISO 9141-2 (5 baud autoinit)");
    ui->canProtocolComboBox->addItem("23:ISO 14230 (no autoinit)");
    ui->canProtocolComboBox->addItem("24:ISO 14230 (5 baud autoinit)");
    ui->canProtocolComboBox->addItem("25:ISO 14230 (fast autoinit)");
    ui->canProtocolComboBox->addItem("31:HS CAN (ISO 11898, 11-bit Tx, 500kbps, var DLC)");
    ui->canProtocolComboBox->addItem("32:HS CAN (ISO 11898, 29-bit Tx, 500kbps, var DLC)");
    ui->canProtocolComboBox->addItem("33:HS CAN (ISO 15765, 11-bit Tx, 500kbps, DLC=8");
    ui->canProtocolComboBox->addItem("34:HS CAN (ISO 15765, 29-bit Tx, 500kbps, DLC=8");
    ui->canProtocolComboBox->addItem("35:HS CAN (ISO 15765, 11-bit Tx, 250kbps, DLC=8");
    ui->canProtocolComboBox->addItem("36:HS CAN (ISO 15765, 29-bit Tx, 250kbps, DLC=8");
    ui->canProtocolComboBox->addItem("41:J1939 (11-bit Tx)");
    ui->canProtocolComboBox->addItem("42:J1939 (29-bit Tx)");
    ui->canProtocolComboBox->addItem("51:MS CAN (ISO 11898, 11-bit Tx, 125kbps, var DLC)");
    ui->canProtocolComboBox->addItem("52:MS CAN (ISO 11898, 29-bit Tx, 125kbps, var DLC)");
    ui->canProtocolComboBox->addItem("53:MS CAN (ISO 15765, 11-bit Tx, 125kbps, DLC=8)");
    ui->canProtocolComboBox->addItem("54:MS CAN (ISO 15765, 29-bit Tx, 125kbps, DLC=8)");
    ui->canProtocolComboBox->addItem("61:SW CAN (ISO 11898, 11-bit Tx, 33.3kbps, var DLC)");
    ui->canProtocolComboBox->addItem("62:SW CAN (ISO 11898, 29-bit Tx, 33.3kbps, var DLC)");
    ui->canProtocolComboBox->addItem("63:SW CAN (ISO 15765, 11-bit Tx, 33.3kbps, DLC=8)");
    ui->canProtocolComboBox->addItem("64:SW CAN (ISO 15765, 29-bit Tx, 33.3kbps, DLC=8)");


    // signal handlers
    connect(ui->canMonitorStartPushButton,SIGNAL(clicked()),this,SLOT(uiStartMonitorClicked()));
    connect(ui->canMonitorStopPushButton,SIGNAL(clicked()),this,SLOT(uiStopMonitorClicked()));
    connect( ui->canStyleComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(uiCanStyleChanged(int)));
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

void MainWindow::uiCanStyleChanged(int index)
{
    m_canDispStyle = index;
    ui->canMsgTableWidget->clear();
    ui->canMsgTableWidget->setColumnCount(0);
    if (m_canDispStyle == 0)
    {
        ui->canMsgTableWidget->setRowCount(1);
    }
    else if (m_canDispStyle == 1)
    {
        ui->canMsgTableWidget->setRowCount(1);
    }
    else if (m_canDispStyle == 2)
    {
        ui->canMsgTableWidget->setRowCount(0);
        ui->canMsgTableWidget->setColumnCount(8);
        ui->canMsgTableWidget->setHorizontalHeaderItem(0,new QTableWidgetItem("CANID"));
        ui->canMsgTableWidget->setHorizontalHeaderItem(1,new QTableWidgetItem("SOURCE"));
        ui->canMsgTableWidget->setHorizontalHeaderItem(2,new QTableWidgetItem("BYTE 0"));
        ui->canMsgTableWidget->setHorizontalHeaderItem(3,new QTableWidgetItem("BYTE 1"));
        ui->canMsgTableWidget->setHorizontalHeaderItem(4,new QTableWidgetItem("BYTE 2"));
        ui->canMsgTableWidget->setHorizontalHeaderItem(5,new QTableWidgetItem("BYTE 3"));
        ui->canMsgTableWidget->setHorizontalHeaderItem(6,new QTableWidgetItem("BYTE 4"));
        ui->canMsgTableWidget->setHorizontalHeaderItem(7,new QTableWidgetItem("BYTE 5"));
    }

}

void MainWindow::uiStartMonitorClicked()
{
    m_canMsgCount = 0;
    QString file =  ui->canSaveLogFileLineEdit->text();
    if (file != "")
    {
        m_canLogFile = new QFile(file);
        m_canLogFile->open(QIODevice::ReadWrite | QIODevice::Append);
    }
    else
    {
        m_canLogFile = 0;
    }

}

void MainWindow::uiStopMonitorClicked()
{
    //todo
}

void MainWindow::uiCanSaveLogFileBrowseClicked()
{
    QFileDialog dialog;
    if (dialog.exec())
    {
        if (dialog.selectedFiles().size() > 0)
        {
            ui->canLoadLogFileLineEdit->setText(dialog.selectedFiles()[0]);
        }
    }
}

