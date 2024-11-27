#pragma once

#include <QWidget>

#include <QSpinBox>
#include <QLineEdit>

class UsbDevice;

class NtpWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NtpWidget(UsbDevice *device, QWidget *parent = nullptr);

private:
    void onDeviceOpened();
    void onDeviceClosed();
    void onReport(const std::string &msg);
    void onTimezoneChanged(int timezone);
    void setServer();
    void syncNtpTime();

    QSpinBox *mTimeZoneEdit = nullptr;
    QLineEdit *mServerEdit = nullptr;
    UsbDevice *mDevice = nullptr;
};
