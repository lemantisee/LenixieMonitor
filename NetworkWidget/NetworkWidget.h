#pragma once

#include <QWidget>

#include <QLabel>
#include <QLineEdit>
#include <QToolButton>

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
    void onConnectAction();
    void onConnectLastAction();
    void onDisconnectAction();
    void onForgetAction();
    void onState(const DeviceReport &report);

    void requestStatus();

    QAction *mConnectAction = nullptr;
    QAction *mConnectToLastAction = nullptr;
    QAction *mDisconnectAction = nullptr;
    QAction *mForgetAction = nullptr;

    QLabel *mStatusLabel = nullptr;
    QToolButton *mMenuButton = nullptr;
    QTimer *mUpdateTimer = nullptr;
    UsbDevice *mDevice = nullptr;
};
