#pragma once

#include <string>

#include "libusb.h"

class UsbDevice
{
public:
    UsbDevice();
    ~UsbDevice();

    bool open(uint32_t vid, uint32_t pid);

    std::string read();

private:
    libusb_device *findDevice(uint32_t vid, uint32_t pid) const;

    bool mUsbContextInited = false;
    libusb_device *mDevice = nullptr;
    libusb_device_handle *mHandle = nullptr;
};
