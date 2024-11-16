#include "UsbWatcher.h"

#include "Logger.h"

UsbWatcher::UsbWatcher(QObject *parent) : QObject(parent) {}

UsbWatcher::~UsbWatcher()
{
    if (!mInited) {
        return;
    }

    stop();
}

bool UsbWatcher::start(int vid, int pid)
{
    int res = libusb_init_context(nullptr, nullptr, 0);
    if (res < 0) {
        LOG_ERROR("Unable to init usb context");
        return false;
    }

    mThread = std::jthread(&UsbWatcher::process, this, vid, pid);
    mInited = true;

    return true;
}

void UsbWatcher::stop()
{
    mThread.request_stop();
    if (mThread.joinable()) {
        mThread.join();
    }

    libusb_exit(nullptr);

    mInited = false;
}

bool UsbWatcher::findDevice(uint32_t vid, uint32_t pid) const
{
    libusb_device **devs = nullptr;
    if (ssize_t cnt = libusb_get_device_list(nullptr, &devs); cnt < 0) {
        LOG_ERROR("Unable to get usb devices list");
        return false;
    }

    for (uint32_t i = 0; devs[i]; i++) {
        libusb_device_descriptor desc;
        libusb_device *device = devs[i];
        if (int ret = libusb_get_device_descriptor(device, &desc); ret < 0) {
            LOG_ERROR("failed to get device descriptor. Error %s", stderr);
            return false;
        }

        if (desc.idVendor == vid && desc.idProduct == pid) {
            libusb_ref_device(device);
            libusb_free_device_list(devs, 1);
            return true;
        }
    }

    libusb_free_device_list(devs, 1);

    return false;
}

void UsbWatcher::process(uint32_t vid, uint32_t pid)
{
    bool deviceState = false;
    auto token = mThread.get_stop_token();
    while (!token.stop_requested()) {
        bool state = findDevice(vid, pid);
        if (deviceState != state) {
            deviceState = state;
            if (state) {
                emit arrived();
            } else {
                emit left();
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
