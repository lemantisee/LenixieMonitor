#include "DataPacket.h"

#include <cstdint>

#include "Logger.h"

namespace {

//2 bytes unique header
//1 byte type
//1 byte size
//60 bytes payload

const uint16_t header = 0x1A56;
const int payloadMaxSize = 60;
const size_t headerIndex = 0;
const size_t typeIndex = 2;
const size_t sizeIndex = 3;
const size_t payloadIndex = 4;
} // namespace

DataPacket::DataPacket(const std::string &payload)
{
    if (payload.size() > payloadMaxSize) {
        LOG_ERROR("Too big payload size");
        return;
    }

    std::memcpy(mData.data() + headerIndex, &header, 2);
    mData[typeIndex] = PacketPayload;
    mData[sizeIndex] = uint8_t(payload.size());
    std::memcpy(mData.data() + payloadIndex, payload.data(), payload.size());
}

void DataPacket::setEnd() { mData[typeIndex] = EndPacket; }

uint8_t *DataPacket::data() { return mData.data(); }

size_t DataPacket::size() const { return mData.size(); }

DataPacket::PacketType DataPacket::getType() const { return PacketType(mData[typeIndex]); }

std::string DataPacket::getPayload() const
{
    const uint8_t payloadSize = mData[sizeIndex];

    if (payloadSize == 0) {
        return {};
    }

    std::string payload;
    payload.resize(payloadSize);

    std::memcpy(payload.data(), &mData[payloadIndex], payloadSize);

    return payload;
}

std::vector<DataPacket> DataPacket::fromString(const std::string &msg)
{
    std::vector<DataPacket> packets;

    size_t size = msg.size();
    size_t pos = 0;

    while (size > 0) {
        size_t sizeToCopy = std::min<size_t>(size, payloadMaxSize);
        std::string token = msg.substr(pos, sizeToCopy);

        DataPacket packet(token);

        packets.push_back(packet);

        size -= sizeToCopy;
        pos += sizeToCopy;
    }

    packets.back().setEnd();
    return packets;
}

DataPacket DataPacket::fromReport(const std::vector<uint8_t> &report)
{
    if (report.size() != 64) {
        return {};
    }

    uint16_t packetHeader = 0;
    std::memcpy(&packetHeader, report.data(), 2);

    if (packetHeader != header) {
        return {};
    }

    PacketType type = PacketType(report[typeIndex]);
    const uint8_t payloadSize = report[sizeIndex];

    if ((payloadSize == 0 && type != PacketAck) || payloadSize > 60) {
        return {};
    }

    DataPacket packet;
    std::memcpy(packet.mData.data(), report.data(), report.size());

    return packet;
}

DataPacket DataPacket::ackPacket()
{
    DataPacket packet;

    std::memcpy(packet.mData.data() + headerIndex, &header, 2);
    packet.mData[typeIndex] = PacketAck;

    return packet;
}
