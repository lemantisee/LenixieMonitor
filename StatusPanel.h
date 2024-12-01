#pragma once

#include <QWidget>

#include <QLabel>

class UsbDevice;

class StatusPanel : public QWidget
{
    Q_OBJECT
public:
    explicit StatusPanel(UsbDevice *device, QWidget *parent = nullptr);

private:
    void onOpen();
    void onReport(const std::string &msg);

    UsbDevice *mDevice = nullptr;
    QLabel *mStatusLabel = nullptr;
    QLabel *mFirmwareVersionLabel = nullptr;
    QLabel *mAtVersion = nullptr;
    QLabel *mSdkVersion = nullptr;
};
