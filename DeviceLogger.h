#pragma once

class UsbDevice;

class DeviceLogger
{
public:
    DeviceLogger();
    bool init(UsbDevice *device);

    void process();

private:
    UsbDevice *mDevice = nullptr;
};
