#pragma once

#include <thread>
#include <functional>

#include "libusb.h"

class UsbWatcher
{
public:
    enum Event { Arrived, Left };

    UsbWatcher();
    ~UsbWatcher();

    bool start(int vid, int pid);

    void onEvent(std::function<void(int vid, int pid, Event event)> func);

private:
    bool findDevice(uint32_t vid, uint32_t pid) const;
    void process(uint32_t vid, uint32_t pid);

    libusb_context *mUsbContext = nullptr;
    bool mInited = false;
    std::jthread mThread;
    std::function<void(int vid, int pid, Event event)> mEventCallback;
};
