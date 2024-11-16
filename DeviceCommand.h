#pragma once

#include <string>

enum PanelCommandId {
    UnknownCommand = 0,
    EchoCommand = 1,
    EnableLog = 2,
    GetLog = 3,
    LogUnit = 4,
    LogUnitEnd = 5,
    LogEnd = 6,
};

struct DeviceReport
{
    PanelCommandId cmd = UnknownCommand;
    std::string data;
};
