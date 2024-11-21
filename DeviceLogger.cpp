#include "DeviceLogger.h"

#include <QTimer>

#include "UsbDevice.h"
#include "json.hpp"

#include "Logger.h"

DeviceLogger::DeviceLogger(UsbDevice *device, QObject *parent) : QObject(parent)
{
    mDevice = device;

    mTimer = new QTimer(this);
    mTimer->setInterval(std::chrono::milliseconds(50));

    connect(mTimer, &QTimer::timeout, this, &DeviceLogger::pullLog);
    connect(device, &UsbDevice::recieved, this, &DeviceLogger::onReport);
}

void DeviceLogger::start()
{
    mTimer->start();
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
    }
}

DeviceReport DeviceLogger::parseReport(const std::string &str) const
{
    nlohmann::json jreport = nlohmann::json::parse(str, nullptr, false);
    if (jreport.empty()) {
        return {};
    }

    DeviceReport report;
    report.cmd = jreport.value("id", UnknownCommand);
    report.data = jreport.value("d", "");

    return report;
}

void DeviceLogger::appendLog(const DeviceReport &report)
{
    switch (report.cmd) {
    case LogUnit:
    case LogUnitEnd: mLogBuffer += report.data; break;
    case LogEnd: mLogBuffer.clear(); break;
    default: break;
    }
}

bool DeviceLogger::requestLog()
{
    nlohmann::json jsonCommand;
    jsonCommand["id"] = GetLog;

    std::string commandStr = jsonCommand.dump();
    while (commandStr.size() < 64) {
        commandStr.push_back(0);
    }

    return mDevice->send(commandStr);
}

void DeviceLogger::onReport(const std::string &msg)
{
    if (msg.empty()) {
        startTimer();
        return;
    }

    DeviceReport report = parseReport(msg);
    appendLog(report);

    if (report.cmd == LogEnd) {
        mLogBuffer.clear();
        startTimer();
        return;
    }

    if (report.cmd == LogUnitEnd) {
        emit logRecived(mLogBuffer);
        mLogBuffer.clear();
    }

    mTimer->stop();

    if (!requestLog()) {
        LOG_ERROR("Unable to send command");
    }
}

void DeviceLogger::startTimer()
{
    if (!mTimer->isActive()) {
        mTimer->start();
    }
}
