#pragma once

#include <string>

#include "json.hpp"

class DeviceReport
{
public:
    enum PanelCommandId {
        UnknownCommand = 0,
        GetDateTime = 1,
        DateTimeState = 2,
        GetLog = 3,
        LogUnit = 4,
        LogEnd = 5,
        SetDateTime = 6,
        GetNetworkState = 7,
        NetworkState = 8,
        ConnectToWifi = 9,
        ConnectToLastWifi = 10,
        DisconnectWifi = 11,
        SetTimezone = 12,
        SetNtpServer = 13,
        SyncNtpTime = 14,
        GetNtpState = 15,
        NtpState = 16,
        GetVersion = 17,
        VersionInfo = 18,
        SetDndHours = 19,
        EnableDnd = 20,
        GetDndState = 21,
        DndState = 22,

        MessageAck = 50,
    };

    DeviceReport() = default;
    DeviceReport(const std::string &report)
    {
        mJsonReport = nlohmann::json::parse(report, nullptr, false);
    }

    DeviceReport(PanelCommandId cmd)
    {
        mJsonReport["id"] = cmd;
    }

    std::string toString() const { return mJsonReport.dump(); }

    template<typename T>
    T get(std::string_view key, T defaultValue) const
    {
        return mJsonReport.value(key, defaultValue);
    }

    template<typename T>
    void set(std::string_view key, T value)
    {
        mJsonReport[key] = value;
    }

    PanelCommandId getCmd() const { return mJsonReport.value("id", UnknownCommand); }

private:
    nlohmann::json mJsonReport;
};
