#pragma once

#include <vector>
#include <string>
#include <array>

class DataPacket
{
public:
    enum PacketType {
        UnknownPacket = 0,
        PacketPayload = 1,
        EndPacket = 2,
        PacketAck = 3,
    };

    DataPacket() = default;
    DataPacket(const std::string &payload);

    void setEnd();
    uint8_t *data();
    size_t size() const;
    PacketType getType() const;
    std::string getPayload() const;

    static std::vector<DataPacket> fromString(const std::string &msg);
    static DataPacket fromReport(const std::vector<uint8_t> &report);
    static DataPacket ackPacket();

private:
    std::array<uint8_t, 64> mData = {};
};
