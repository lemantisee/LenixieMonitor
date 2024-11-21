#include "UsbDevice.h"

#include "Logger.h"

UsbDevice::UsbDevice(QObject *parent) : QObject(parent)
{
}

UsbDevice::~UsbDevice()
{
    mThread.request_stop();
    if (mThread.joinable()) {
        mThread.join();
    }

    closeDevice();

    if (mUsbContext) {
        libusb_exit(mUsbContext);
        mUsbContext = nullptr;
    }
}

bool UsbDevice::open(uint32_t vid, uint32_t pid)
{
    mVid = vid;
    mPid = pid;
    int res = libusb_init_context(&mUsbContext, nullptr, 0);
    if (res < 0 || !mUsbContext) {
        LOG_ERROR("Unable to init usb context");
        return false;
    }

    mThread = std::jthread(&UsbDevice::process, this);

    return true;
}

bool UsbDevice::isOpened() const
{
    std::lock_guard lock(mMutex);

    return mDevice != nullptr;
}

bool UsbDevice::send(const std::string &data)
{
    std::lock_guard lock(mMutex);

    if (!mDevice) {
        return false;
    }

    mOutMessages.push(data);
    return true;
}

std::string UsbDevice::read()
{
    std::string buffer;
    buffer.resize(64);

    int res = libusb_interrupt_transfer(mHandle, 0x81, (unsigned char *)buffer.data(),
                                        int(buffer.size()), nullptr, 100);

    switch (res) {
    case LIBUSB_ERROR_TIMEOUT: return {};
    case LIBUSB_SUCCESS: return buffer;
    default: break;
    }

    LOG_ERROR("Unable to recieve data. Code %i. %s", res, libusb_strerror(res));

    return {};
}

std::string UsbDevice::popMessage()
{
    if (mOutMessages.empty()) {
        return {};
    }

    std::lock_guard lock(mMutex);
    std::string msg = mOutMessages.front();
    mOutMessages.pop();

    return msg;
}

bool UsbDevice::write(const std::string &data)
{
    int res = libusb_interrupt_transfer(mHandle, 0x01, (unsigned char *)data.data(),
                                        int(data.size()), nullptr, 100);
    switch (res) {
    case LIBUSB_ERROR_TIMEOUT: return false;
    case LIBUSB_SUCCESS: return true;
    default: break;
    }

    LOG_ERROR("Unable to send data. Code %i. %s", res, libusb_strerror(res));
    return false;
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

void UsbDevice::process()
{
    auto stopToken = mThread.get_stop_token();
    while (!stopToken.stop_requested()) {
        libusb_device *currentDevice = findDevice(mVid, mPid);
        const bool left = mDevice && !currentDevice;
        const bool arrived = !mDevice && currentDevice;
        if (left) {
            onLeft();
        }

        if (arrived) {
            onArrived();
        }

        if (!mDevice) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            continue;
        }

        std::string outMsg = popMessage();
        if (!outMsg.empty()) {
            write(outMsg);
        }

        std::string inMsg = read();

        if (!inMsg.empty()) {
            emit recieved(inMsg);
        }
    }
}

void UsbDevice::onArrived()
{
    libusb_device *device = findDevice(mVid, mPid);
    if(!openDevice(device)){
        LOG_ERROR("Unable to open device");
        return;
    }

    mDevice = device;
    emit opened();
}

void UsbDevice::onLeft()
{
    closeDevice();
    emit closed();
}
