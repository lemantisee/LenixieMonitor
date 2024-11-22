#pragma once

#include <QMainWindow>

class UsbDevice;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    QWidget *buildTabWidget();

    UsbDevice *mUsbDevice = nullptr;
};
