#pragma once

#include <string>

#include "json.hpp"

#include "Logger.h"

enum PanelCommandId {
    UnknownCommand = 0,
    GetDateTime = 1,
    DateTimeState = 2,
    GetLog = 3,
    LogUnit = 4,
    LogEnd = 6,
    SetDateTime = 7,
};

class DeviceReport
{
public:
    DeviceReport() = default;
    DeviceReport(const std::string &report)
    {
        nlohmann::json jreport = nlohmann::json::parse(report, nullptr, false);
        if (jreport.empty()) {
            return;
        }

        cmd = jreport.value("id", UnknownCommand);
        data = jreport.value("d", "");
    }

    DeviceReport(PanelCommandId cmd_) { cmd = cmd_; }
    DeviceReport(PanelCommandId cmd_, std::string str)
    {
        cmd = cmd_;
        data = std::move(str);
    }

    std::string toString() const
    {
        nlohmann::json jsonCommand;
        jsonCommand["id"] = cmd;
        if (!data.empty()) {
            jsonCommand["d"] = data;
        }

        std::string commandStr = jsonCommand.dump();
        if (commandStr.size() > 64) {
            LOG_ERROR("To big data payload");
            return {};
        }

        while (commandStr.size() < 64) {
            commandStr.push_back(0);
        }

        return commandStr;
    }

    PanelCommandId cmd = UnknownCommand;
    std::string data;
};
