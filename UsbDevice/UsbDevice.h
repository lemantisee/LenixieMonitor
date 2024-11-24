#pragma once

#include <QObject>

#include <string>
#include <thread>
#include <mutex>
#include <queue>

#include "libusb.h"

class DataPacket;

class UsbDevice : public QObject
{
    Q_OBJECT
public:
    UsbDevice(QObject *parent);
    ~UsbDevice();

    bool open(uint32_t vid, uint32_t pid);
    bool isOpened() const;
    bool send(const std::string &data);

signals:
    void opened();
    void closed();
    void recieved(const std::string &msg);

private:
    libusb_device *findDevice(uint32_t vid, uint32_t pid) const;
    bool openDevice(libusb_device *device);
    void closeDevice();
    void process();
    std::vector<uint8_t> read() const;
    bool write(DataPacket packet);
    std::string popMessage();
    void readSession();

    void onArrived();
    void onLeft();

    libusb_context *mUsbContext = nullptr;
    libusb_device *mDevice = nullptr;
    libusb_device_handle *mHandle = nullptr;
    std::jthread mThread;
    std::queue<std::string> mOutMessages;
    std::string mInMessage;
    mutable std::mutex mMutex;

    int mPid = 0;
    int mVid = 0;
};
