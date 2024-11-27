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
        return;
    }

    DeviceReport request(DeviceReport::GetLog);
    if (!mDevice->send(request.toString())) {
        LOG_ERROR("Unable to send command");
    }
}

void DeviceLogger::onReport(const std::string &msg)
{
    DeviceReport report(msg);

    switch (report.getCmd()) {
    case DeviceReport::LogUnit:
        emit logRecived(report.get("d", std::string()));
        break;
    default:
        break;
    }
}

void DeviceLogger::onDeviceOpened()
{
    mTimer->start();
}

void DeviceLogger::onDeviceClosed()
{
    mTimer->stop();
}
