#include "UsbDevice.h"

#include "Logger.h"

UsbDevice::UsbDevice()
{
    mWatcher.onEvent(
        [this](int vid, int pid, UsbWatcher::Event event) { onWatcherCallback(vid, pid, event); });
}

UsbDevice::~UsbDevice()
{
    closeDevice();

    if (mUsbContext) {
        libusb_exit(mUsbContext);
        mUsbContext = nullptr;
    }
}

bool UsbDevice::open(uint32_t vid, uint32_t pid)
{
    int res = libusb_init_context(&mUsbContext, nullptr, 0);
    if (res < 0 || !mUsbContext) {
        LOG_ERROR("Unable to init usb context");
        return false;
    }

    if (!mWatcher.start(vid, pid)) {
        LOG_ERROR("Unable to start usb watcher");
        return false;
    }

    return true;
}

bool UsbDevice::isOpened() const
{
    std::lock_guard lock(mMutex);

    return mDevice != nullptr;
}

std::string UsbDevice::read()
{
    std::lock_guard lock(mMutex);

    if (!mDevice) {
        return {};
    }

    std::string buffer;
    buffer.resize(64);

    if (int res = libusb_interrupt_transfer(mHandle,
                                            0x81,
                                            (unsigned char *) buffer.data(),
                                            int(buffer.size()),
                                            nullptr,
                                            0);
        res != LIBUSB_SUCCESS ) {
        LOG_ERROR("Unable to recieve data %i", res);
        return {};
    }

    return buffer;
}

bool UsbDevice::write(const std::string &data)
{
    std::lock_guard lock(mMutex);

    if (!mDevice) {
        return false;
    }

    if (int res = libusb_interrupt_transfer(mHandle,
                                            0x01,
                                            (unsigned char *) data.data(),
                                            int(data.size()),
                                            nullptr,
                                            0);
        res != LIBUSB_SUCCESS ) {
        LOG_ERROR("Unable to send data. Code %i. %s", res, libusb_strerror(res));
        return false;
    }

    return true;
}

libusb_device *UsbDevice::findDevice(uint32_t vid, uint32_t pid) const
{
    libusb_device **devs = nullptr;
    if (ssize_t cnt = libusb_get_device_list(nullptr, &devs); cnt < 0) {
        LOG_ERROR("Unable to get usb devices list");
        return nullptr;
    }

    for (uint32_t i = 0; devs[i]; i++) {
        libusb_device_descriptor desc;
        libusb_device *device = devs[i];
        if (int ret = libusb_get_device_descriptor(device, &desc); ret < 0) {
            LOG_ERROR("failed to get device descriptor. Error %s", stderr);
            return nullptr;
        }

        if (desc.idVendor == vid && desc.idProduct == pid) {

            libusb_ref_device(device);
            libusb_free_device_list(devs, 1);
            return device;
        }
    }

    libusb_free_device_list(devs, 1);
    return nullptr;
}

bool UsbDevice::openDevice(libusb_device *device)
{
    if (int res = libusb_open(device, &mHandle); res < 0) {
        LOG_ERROR("Unable to open device");
        return false;
    }

    if (int res = libusb_set_configuration(mHandle, 1); res < 0) {
        LOG_ERROR("Unable to configurate device");
        return false;
    }

    if (int res = libusb_claim_interface(mHandle, 0); res < 0) {
        LOG_ERROR("Unable to claim interface");
        return false;
    }

    return true;
}

void UsbDevice::closeDevice()
{
    if (mHandle) {
        libusb_release_interface(mHandle, 0);
        libusb_close(mHandle);
    }

    mHandle = nullptr;

    if (mDevice) {
        libusb_unref_device(mDevice);
    }

    mDevice = nullptr;
}

void UsbDevice::onWatcherCallback(uint32_t vid, uint32_t pid, UsbWatcher::Event event)
{
    std::lock_guard lock(mMutex);

    switch (event) {
    case UsbWatcher::Arrived:
        LOG("lenixie arrived");
        mDevice = findDevice(vid, pid);
        openDevice(mDevice);
        break;
    case UsbWatcher::Left:
        LOG("lenixie left");
        closeDevice();
        break;
    default: break;
    }
}
