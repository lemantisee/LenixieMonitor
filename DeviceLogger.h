#pragma once

#include <QObject>

#include <string>

#include "DeviceCommand.h"

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
    DeviceReport parseReport(const std::string &str) const;
    bool requestLog();
    void onReport(const std::string &msg);
    void onDeviceOpened();
    void onDeviceClosed();

    void startTimer();

    UsbDevice *mDevice = nullptr;
    QTimer *mTimer = nullptr;
    std::string mLogBuffer;
};
