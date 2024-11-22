#pragma once

#include <QWidget>

#include <QLabel>
#include <QTimer>
#include <QDateTimeEdit>

class UsbDevice;

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
    void onDateTime(const std::string &data);
    void onSetDateTime();

    QString getTimeString(const std::string &data) const;
    QString getDateString(const std::string &data) const;

    QLabel *mTimeLabel = nullptr;
    QLabel *mDateLabel = nullptr;
    QDateTimeEdit *mDateEdit = nullptr;

    QTimer *mUpdateTimer = nullptr;
    UsbDevice *mDevice = nullptr;
};
