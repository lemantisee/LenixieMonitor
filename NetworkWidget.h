#pragma once

#include <QWidget>

#include <QLabel>
#include <QLineEdit>

class UsbDevice;
class DeviceReport;

class NetworkWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NetworkWidget(UsbDevice *device, QWidget *parent = nullptr);

private:
    void onDeviceOpened();
    void onDeviceClosed();
    void onReport(const std::string &msg);
    void onConnectClicked();
    void onState(const DeviceReport &report);

    void requestStatus();

    QLabel *mStatusLabel = nullptr;
    QLineEdit *mSsidEdit = nullptr;
    QLineEdit *mPassEdit = nullptr;
    QTimer *mUpdateTimer = nullptr;
    UsbDevice *mDevice = nullptr;
};
