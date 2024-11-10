#pragma once

#include <string>
#include <mutex>

#include "libusb.h"

#include "UsbWatcher.h"

class UsbDevice
{
public:
    UsbDevice();
    ~UsbDevice();

    bool open(uint32_t vid, uint32_t pid);
    bool isOpened() const;

    std::string read();
    bool write(const std::string &data);

private:
    libusb_device *findDevice(uint32_t vid, uint32_t pid) const;
    bool openDevice(libusb_device *device);
    void closeDevice();
    void onWatcherCallback(uint32_t vid, uint32_t pid, UsbWatcher::Event event);

    libusb_context *mUsbContext = nullptr;
    libusb_device *mDevice = nullptr;
    libusb_device_handle *mHandle = nullptr;
    mutable std::mutex mMutex;

    UsbWatcher mWatcher;
};
