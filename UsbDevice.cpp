#include "UsbDevice.h"

#include "Logger.h"

UsbDevice::UsbDevice() {}

UsbDevice::~UsbDevice()
{
    libusb_release_interface(mHandle, 0);

    if (mHandle) {
        libusb_close(mHandle);
    }

    if (mDevice) {
        libusb_unref_device(mDevice);
    }

    if (mUsbContextInited) {
        libusb_exit(nullptr);
    }
}

bool UsbDevice::open(uint32_t vid, uint32_t pid)
{
    int res = libusb_init_context(/*ctx=*/nullptr, /*options=*/nullptr, /*num_options=*/0);
    if (res < 0) {
        LOG_ERROR("Unable to init usb context");
        return false;
    }

    mUsbContextInited = true;

    mDevice = findDevice(vid, pid);
    if (!mDevice) {
        LOG_ERROR("Unable to find device");
        return false;
    }

    if (int res = libusb_open(mDevice, &mHandle); res < 0) {
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

std::string UsbDevice::read()
{
    std::string buffer;
    buffer.resize(64);
    int recievedLength = 0;

    if (int res = libusb_interrupt_transfer(mHandle,
                                            0x81,
                                            (unsigned char *) buffer.data(),
                                            int(buffer.size()),
                                            &recievedLength,
                                            0);
        res != LIBUSB_SUCCESS ) {
        LOG_ERROR("Unable to recieve data %i", res);
        return {};
    }

    return buffer;
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
