#include <QCoreApplication>

#include "UsbDevice.h"
#include "DeviceLogger.h"

#include "Logger.h"

namespace {
const uint32_t vendorId = 0x0483;
const uint32_t productId = 0x5750;


} // namespace

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    UsbDevice usbDevice;

    if (!usbDevice.open(vendorId, productId)) {
        LOG_ERROR("Unable to open device");
        return 1;
    }

    DeviceLogger devLogger;

    if (!devLogger.init(&usbDevice)) {
        LOG_ERROR("Unable to init device logger client");
        return 1;
    }

    while (true) {
        devLogger.process();
    }

    return a.exec();
}
