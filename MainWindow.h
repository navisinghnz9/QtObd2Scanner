#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Settings/SettingsWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void OnSettingsMenuClicked();
    void onSettingsChanged(QString port, int baud);

private:
    Ui::MainWindow *ui;
    SettingsWidget settingsWidget;

    QString m_port;
    int m_baud;
};
#endif // MAINWINDOW_H
