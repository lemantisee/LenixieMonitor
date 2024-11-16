#pragma once

#include <QObject>

#include <string>

#include "libusb.h"

#include "UsbWatcher.h"

class UsbDevice : public QObject
{
    Q_OBJECT
public:
    UsbDevice(QObject *parent);
    ~UsbDevice();

    bool open(uint32_t vid, uint32_t pid);
    bool isOpened() const;

    std::string read();
    bool write(const std::string &data);

signals:
    void opened();
    void closed();

private:
    libusb_device *findDevice(uint32_t vid, uint32_t pid) const;
    bool openDevice(libusb_device *device);
    void closeDevice();

    void onArrived();
    void onLeft();

    libusb_context *mUsbContext = nullptr;
    libusb_device *mDevice = nullptr;
    libusb_device_handle *mHandle = nullptr;

    UsbWatcher *mWatcher = nullptr;
    int mPid = 0;
    int mVid = 0;
};
