#include <QCoreApplication>

#include <QDebug>
#include "Logger.h"

#include "libusb.h"

// Device configuration and interface id.
#define MY_CONFIG 1
#define MY_INTF 0

// Device endpoint(s)
#define EP_IN 0x81
#define EP_OUT 0x01

namespace {
const uint32_t vendorId = 0x0483;
const uint32_t productId = 0x5750;

static void print_device(libusb_device *dev, libusb_device_handle *handle)
{
    std::string speed;

    switch (libusb_get_device_speed(dev)) {
    case LIBUSB_SPEED_LOW:
        speed = "1.5M";
        break;
    case LIBUSB_SPEED_FULL:
        speed = "12M";
        break;
    case LIBUSB_SPEED_HIGH:
        speed = "480M";
        break;
    case LIBUSB_SPEED_SUPER:
        speed = "5G";
        break;
    case LIBUSB_SPEED_SUPER_PLUS:
        speed = "10G";
        break;
    default:
        speed = "Unknown";
    }


    libusb_device_descriptor desc;
    if (int ret = libusb_get_device_descriptor(dev, &desc); ret < 0) {
        LOG_ERROR("failed to get device descriptor. Error %s", stderr);
        return;
    }

    LOG("Dev (bus %i, device %i): %i - %i speed: %s",
        libusb_get_bus_number(dev),
        libusb_get_device_address(dev),
        desc.idVendor,
        desc.idProduct,
        speed);
}

libusb_device *findDevice(uint32_t vid, uint32_t pid)
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

} // namespace

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    int res = libusb_init_context(/*ctx=*/nullptr, /*options=*/nullptr, /*num_options=*/0);
    if (res < 0) {
        LOG_ERROR("Unable to init usb context");
        return 1;
    }

    libusb_device *device = findDevice(vendorId, productId);
    if (!device) {
        LOG_ERROR("Unable to find device");
        libusb_exit(nullptr);
        return 1;
    }

    print_device(device, nullptr);

    libusb_device_handle *handle = nullptr;
    if (int res = libusb_open(device, &handle); res < 0) {
        LOG_ERROR("Unable to open device");
        libusb_unref_device(device);
        libusb_exit(nullptr);
        return 1;
    }

    if (int res = libusb_set_configuration(handle, 1); res < 0) {
        LOG_ERROR("Unable to configurate device");
        libusb_unref_device(device);
        libusb_exit(nullptr);
        return 1;
    }

    if (int res = libusb_claim_interface(handle, 0); res < 0) {
        LOG_ERROR("Unable to claim interface");
        libusb_unref_device(device);
        libusb_exit(nullptr);
        return 1;
    }


    while (true) {
        std::string buffer;
        buffer.resize(64);
        int recievedLength = 0;

        if (int res = libusb_interrupt_transfer(handle,
                                                0x81,
                                                (unsigned char *) buffer.data(),
                                                int(buffer.size()),
                                                &recievedLength,
                                                0);
            res != LIBUSB_SUCCESS ) {
            LOG_ERROR("Unable to recieve data %i", res);
            libusb_unref_device(device);
            libusb_exit(nullptr);
            return 1;
        }

        LOG(buffer.c_str());
    }


    libusb_release_interface(handle, 0);

    if (handle) {
        libusb_close(handle);
    }

    libusb_unref_device(device);

    libusb_exit(nullptr);

    return a.exec();
}
