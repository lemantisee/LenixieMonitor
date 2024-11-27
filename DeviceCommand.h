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
        LogEnd = 6,
        SetDateTime = 7,
        GetNetworkState = 8,
        NetworkState = 9,
        ConnectToWifi = 10,
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
        return mJsonReport.value(key, T());
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
