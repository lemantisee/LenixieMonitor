cmake_minimum_required(VERSION 3.14)

project(LenixieMonitor LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_executable(${PROJECT_NAME}
    main.cpp

    json/json.hpp
    UsbDevice.h UsbDevice.cpp
    DeviceLogger.h DeviceLogger.cpp
    Logger.h
    DeviceCommand.h
    UsbWatcher.h UsbWatcher.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/libusb
    ${CMAKE_CURRENT_SOURCE_DIR}/json
)

target_link_libraries(${PROJECT_NAME}
    Qt${QT_VERSION_MAJOR}::Core
    ${CMAKE_CURRENT_SOURCE_DIR}/libusb/libusb-1.0.lib
)
