target_sources(${PROJECT_NAME} PRIVATE
    UsbDevice.h UsbDevice.cpp
    UsbWatcher.h UsbWatcher.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
