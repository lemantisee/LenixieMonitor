#include <QCoreApplication>

#include "Logger.h"

#include "json.hpp"
#include "UsbDevice.h"

namespace {

const uint32_t vendorId = 0x0483;
const uint32_t productId = 0x5750;

} // namespace

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    UsbDevice usbDevice;

    if  (!usbDevice.open(vendorId, productId)) {
        LOG_ERROR("Unable to open device");
        return 1;
    }

    // print_device(device, nullptr);

    while (true) {
        std::string buffer = usbDevice.read();
        LOG(buffer.c_str());
    }

    return a.exec();
}
