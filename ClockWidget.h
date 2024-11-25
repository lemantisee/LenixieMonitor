#pragma once

#include <QWidget>

#include <QLabel>
#include <QTimer>
#include <QDateTimeEdit>

class UsbDevice;
class DeviceReport;

class ClockWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ClockWidget(UsbDevice *device, QWidget *parent = nullptr);

private:
    bool requestDateTime();

    void onDeviceOpened();
    void onDeviceClosed();
    void onReport(const std::string &msg);
    void onDateTime(const DeviceReport &report);
    void onSetDateTime();

    QString getTimeString(const DeviceReport &report) const;
    QString getDateString(const DeviceReport &report) const;

    QLabel *mTimeLabel = nullptr;
    QLabel *mDateLabel = nullptr;
    QDateTimeEdit *mDateEdit = nullptr;

    QTimer *mUpdateTimer = nullptr;
    UsbDevice *mDevice = nullptr;
};
