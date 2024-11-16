#pragma once

#include <QMainWindow>

#include <QLabel>

class UsbDevice;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    void onDeviceOpened();
    void onDeviceClosed();

    QLabel *mLabel = nullptr;

    UsbDevice *mUsbDevice = nullptr;
};
