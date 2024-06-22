#pragma once

#include <string>

#include <QDebug>

#define __FILENAME__ strrchr("\\" __FILE__, '\\') + 1

#define LOG(msg, ...) Logger::log(Logger::Info, __FILENAME__ , __LINE__, msg, __VA_ARGS__)
#define LOG_ERROR(msg, ...) Logger::log(Logger::Error, __FILENAME__ , __LINE__, msg, __VA_ARGS__)

class Logger
{
public:
    enum Type { Info, Error };
    Logger();

    template<typename... Args>
    static void log(Type type, const char* file, int line, const char *fmt, Args... args) {
        std::string logString = stringFormat("[%s:%i]", file, line);

        if (type == Error) {
            logString += "ERROR:";
        }

        logString += stringFormat(fmt, args...);
        // std::printf("%s\n", logString.c_str());
        qDebug() << logString.c_str();
    }

private:
    template<typename... Args>
    static std::string stringFormat(const char *fmt, Args... args)
    {
        int size_s = std::snprintf(nullptr, 0, fmt, args...) + 1; // Extra space for '\0'
        if (size_s <= 0) {
            return {};
        }
        std::string str;
        str.resize(size_s);
        std::snprintf(str.data(), str.size(), fmt, args...);
        str.pop_back(); // We don't want the '\0' inside
        return str;
    }
};
