#pragma once

#include <string>

class UsbDevice;

class DeviceLogger
{
public:
    DeviceLogger();
    bool init(UsbDevice *device);

    void process();

private:
    std::string parseLog(std::string str);

    UsbDevice *mDevice = nullptr;
    std::string mLogBuffer;
};
