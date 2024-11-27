#pragma once

#include <QObject>

#include <string>

class UsbDevice;

class DeviceLogger : public QObject
{
    Q_OBJECT
public:
    DeviceLogger(UsbDevice *device, QObject *parent);

signals:
    void logRecived(const std::string &str);

private:
    void pullLog();
    void onReport(const std::string &msg);
    void onDeviceOpened();
    void onDeviceClosed();

    UsbDevice *mDevice = nullptr;
    QTimer *mTimer = nullptr;
};
