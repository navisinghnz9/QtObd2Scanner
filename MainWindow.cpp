#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionSettings, SIGNAL(triggered()), this, SLOT(OnSettingsMenuClicked()));
    m_port = "comport";
    m_baud = 0;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::OnSettingsMenuClicked()
{
    settingsWidget = new SettingsWidget();
    connect(settingsWidget,SIGNAL(saveSettings(QString,int)),this,SLOT(settings_saveComPort(QString,int)));
    settingsWidget->setSettings(m_port, m_baud);
    settingsWidget->show();
}
