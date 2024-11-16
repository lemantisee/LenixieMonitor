#pragma once

#include <QObject>

#include <thread>

#include "libusb.h"

class UsbWatcher : public QObject
{
    Q_OBJECT
public:
    UsbWatcher(QObject *parent);
    ~UsbWatcher();

    bool start(int vid, int pid);
    void stop();

signals:
    void arrived();
    void left();

private:
    bool findDevice(uint32_t vid, uint32_t pid) const;
    void process(uint32_t vid, uint32_t pid);

    libusb_context *mUsbContext = nullptr;
    bool mInited = false;
    std::jthread mThread;
};
