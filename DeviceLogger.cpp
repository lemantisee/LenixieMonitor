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
    LOG(buffer.c_str());
}


