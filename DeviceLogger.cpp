#include "DeviceLogger.h"

#include "UsbDevice.h"
#include "json.hpp"
#include "DeviceCommand.h"

#include "Logger.h"


DeviceLogger::DeviceLogger() {}

bool DeviceLogger::init(UsbDevice *device)
{
    mDevice = device;
    return true;
}

void DeviceLogger::process()
{
    if(!mDevice) {
        LOG_ERROR("Ivalid device");
        return;
    }

    if(!mDevice->isOpened()) {
        return;
    }

    nlohmann::json jsonCommand;
    jsonCommand["id"] = GetLog;

    std::string commandStr = jsonCommand.dump();
    while (commandStr.size() < 64 ) {
        commandStr.push_back(0);
    }

    if (!mDevice->write(commandStr)) {
        LOG_ERROR("Unable to send command");
        return;
    }

    std::string buffer = mDevice->read();

    if (buffer.empty()) {
        LOG("Empty buffer");
        return;
    }

    std::string log = parseLog(std::move(buffer));

    if (!log.empty()) {
        LOG("\n\t%s", log.c_str());
    }
}

std::string DeviceLogger::parseLog(std::string str)
{
    nlohmann::json jLog = nlohmann::json::parse(str, nullptr, false);
    if (jLog.empty()) {
        return {};
    }

    PanelCommandId id = jLog.value("id", UnknownCommand);

    switch (id) {
    case LogUnit:
        mLogBuffer += jLog.value("d", "");
        break;
    case LogUnitEnd: {
        std::string log = mLogBuffer + jLog.value("d", "");
        mLogBuffer.clear();
        return log;
    }
    case LogEnd:
        mLogBuffer.clear();
        break;
    default:
        break;
    }

    return {};
}
