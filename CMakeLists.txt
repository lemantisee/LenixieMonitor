cmake_minimum_required(VERSION 3.14)

project(LenixieMonitor LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets)

add_executable(${PROJECT_NAME}
    main.cpp

    json/json.hpp
    DeviceLogger.h DeviceLogger.cpp
    Logger.h
    DeviceCommand.h
    MainWindow.h MainWindow.cpp
    LogWidget.h LogWidget.cpp
)

add_subdirectory(UsbDevice)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/libusb
    ${CMAKE_CURRENT_SOURCE_DIR}/json
)

target_link_libraries(${PROJECT_NAME}
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    ${CMAKE_CURRENT_SOURCE_DIR}/libusb/libusb-1.0.lib
)
