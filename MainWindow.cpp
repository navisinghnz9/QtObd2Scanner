#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QSettings>

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
