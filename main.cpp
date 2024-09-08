#include <QCoreApplication>

#include "UsbDevice.h"
#include "DeviceLogger.h"
#include "json.hpp"

#include "Logger.h"

namespace {
const uint32_t vendorId = 0x0483;
const uint32_t productId = 0x5750;
} // namespace

class JsonObject
{
public:
    JsonObject()
    {
        mBuffer.resize(64);
        mBuffer.front() = '{';
        mCuurentPos = 1;
    }

    void add(const char *key, int value)
    {
        if(mCuurentPos > 1) {
            mBuffer[mCuurentPos] += ',';
            ++mCuurentPos;
        }

        addString(key);

        mBuffer[mCuurentPos] = ':';
        ++mCuurentPos;

        std::snprintf(&mBuffer[mCuurentPos], mBuffer.size() - mCuurentPos, "%i", value);
        ++mCuurentPos;
    }

    void add(const char *key, const char *value)
    {
        if(mCuurentPos > 1) {
            mBuffer[mCuurentPos] += ',';
            ++mCuurentPos;
        }

        addString(key);

        mBuffer[mCuurentPos] = ':';
        ++mCuurentPos;

        addString(value);
    }

    std::string dump() {
        if(mCuurentPos >= mBuffer.size()) {
            return {};
        }

        mBuffer[mCuurentPos] += '}';
        ++mCuurentPos;

        return mBuffer;
    }

private:
    void addString(const char *value)
    {
        mBuffer[mCuurentPos] = '\"';
        ++mCuurentPos;

        std::strcpy(mBuffer.data() + mCuurentPos, value);
        mCuurentPos += std::strlen(value);

        mBuffer[mCuurentPos] = '\"';
        ++mCuurentPos;
    }

    size_t mCuurentPos = 0;
    std::string mBuffer;
};

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
