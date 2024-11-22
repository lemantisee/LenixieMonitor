#include "DeviceLogger.h"

#include <QTimer>

#include "DeviceCommand.h"
#include "UsbDevice.h"

#include "Logger.h"

DeviceLogger::DeviceLogger(UsbDevice *device, QObject *parent) : QObject(parent)
{
    mDevice = device;

    mTimer = new QTimer(this);
    mTimer->setInterval(std::chrono::milliseconds(50));

    connect(mTimer, &QTimer::timeout, this, &DeviceLogger::pullLog);

    connect(device, &UsbDevice::opened, this, &DeviceLogger::onDeviceOpened);
    connect(device, &UsbDevice::closed, this, &DeviceLogger::onDeviceClosed);
    connect(device, &UsbDevice::recieved, this, &DeviceLogger::onReport);
}

void DeviceLogger::pullLog()
{
    if (!mDevice) {
        LOG_ERROR("Ivalid device");
        return;
    }

    if (!mDevice->isOpened()) {
        startTimer();
        return;
    }

    if (!requestLog()) {
        LOG_ERROR("Unable to send command");
        startTimer();
        return;
    }
}

bool DeviceLogger::requestLog()
{
    DeviceReport request(GetLog);
    return mDevice->send(request.toString());
}

void DeviceLogger::onReport(const std::string &msg)
{
    DeviceReport report(msg);

    switch (report.cmd) {
    case LogUnit:
        mLogBuffer += report.data;
        break;
    case LogUnitEnd:
        mLogBuffer += report.data;
        emit logRecived(mLogBuffer);
        mLogBuffer.clear();
        break;
    case LogEnd:
        mLogBuffer.clear();
        startTimer();
        return;
    default:
        return;
    }

    if (!requestLog()) {
        startTimer();
        LOG_ERROR("Unable to send command");
    }
}

void DeviceLogger::onDeviceOpened()
{
    mLogBuffer.clear();
    pullLog();
}

void DeviceLogger::onDeviceClosed()
{
    mTimer->stop();
    mLogBuffer.clear();
}

void DeviceLogger::startTimer()
{
    if (!mTimer->isActive()) {
        mTimer->start();
    }
}
