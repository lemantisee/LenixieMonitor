#include <QCoreApplication>

#include "Logger.h"

#include "json.hpp"
#include "UsbDevice.h"

namespace {

const uint32_t vendorId = 0x0483;
const uint32_t productId = 0x5750;

enum PanelCommandId {
    EchoCommand = 1,
};

} // namespace

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    UsbDevice usbDevice;

    if (!usbDevice.open(vendorId, productId)) {
        LOG_ERROR("Unable to open device");
        return 1;
    }

    // print_device(device, nullptr);

    nlohmann::json jsonCommand;
    jsonCommand["id"] = EchoCommand;
    jsonCommand["data"] = "Hello World";

    std::string commandStr = jsonCommand.dump();
    while (commandStr.size() < 64 ) {
        commandStr.push_back(0);
    }

    while (true) {
        LOG("send command");
        if (!usbDevice.write(commandStr)) {
            LOG_ERROR("Unable to send command");
            break;
        }


        LOG("read command");
        std::string buffer = usbDevice.read();
        LOG(buffer.c_str());
    }

    return a.exec();
}
