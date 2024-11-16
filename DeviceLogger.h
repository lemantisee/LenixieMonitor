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
    void start();

signals:
    void logRecived(const std::string &str);

private:

    void pullLog();
    DeviceReport parseReport(const std::string &str) const;
    void appendLog(const DeviceReport &report);
    bool requestLog();

    UsbDevice *mDevice = nullptr;
    QTimer *mTimer = nullptr;
    std::string mLogBuffer;
};
