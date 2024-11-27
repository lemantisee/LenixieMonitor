#include "ClockWidget.h"

#include <QFormLayout>
#include <QLabel>
#include <QTime>
#include <QDateTimeEdit>
#include <QPushButton>

#include "DeviceReport.h"
#include "UsbDevice.h"


ClockWidget::ClockWidget(UsbDevice *device, QWidget *parent) : QWidget{parent}
{
    mDevice = device;

    mUpdateTimer = new QTimer(this);
    mUpdateTimer->setInterval(std::chrono::milliseconds(500));

    QVBoxLayout *main_l = new QVBoxLayout(this);

    QFormLayout *dateState_l = new QFormLayout;
    main_l->addLayout(dateState_l);
    mTimeLabel = new QLabel(tr("Unknown"));
    dateState_l->addRow(tr("Time:"), mTimeLabel);

    mDateLabel = new QLabel(tr("Unknown"));
    dateState_l->addRow(tr("Date:"), mDateLabel);

    QHBoxLayout *setDate_l = new QHBoxLayout;
    main_l->addLayout(setDate_l);
    main_l->addStretch();

    mDateEdit = new QDateTimeEdit;
    mDateEdit->setCalendarPopup(true);
    mDateEdit->setDateTime(QDateTime::currentDateTime());
    mDateEdit->setMinimumWidth(120);

    QPushButton *setDateButton = new QPushButton(tr("Set"));
    setDate_l->addWidget(mDateEdit);
    setDate_l->addWidget(setDateButton);
    setDate_l->addStretch();

    connect(mUpdateTimer, &QTimer::timeout, this, &ClockWidget::requestDateTime);

    connect(mDevice, &UsbDevice::opened, this, &ClockWidget::onDeviceOpened);
    connect(mDevice, &UsbDevice::closed, this, &ClockWidget::onDeviceClosed);
    connect(mDevice, &UsbDevice::recieved, this, &ClockWidget::onReport);

    connect(setDateButton, &QPushButton::clicked, this, &ClockWidget::onSetDateTime);
}

bool ClockWidget::requestDateTime()
{
    if (!mDevice->isOpened()) {
        return true;
    }

    DeviceReport report(DeviceReport::GetDateTime);
    return mDevice->send(report.toString());
}

void ClockWidget::onDeviceOpened()
{
    requestDateTime();
    mUpdateTimer->start();
}

void ClockWidget::onDeviceClosed()
{
    mUpdateTimer->stop();
    mTimeLabel->setText(tr("Unknown"));
    mDateLabel->setText(tr("Unknown"));
}

void ClockWidget::onReport(const std::string &msg)
{
    DeviceReport report(msg);

    switch (report.getCmd()) {
    case DeviceReport::DateTimeState: onDateTime(report); break;
    default: break;
    }
}

void ClockWidget::onDateTime(const DeviceReport &report)
{
    mTimeLabel->setText(getDateString(report) + " " + getTimeString(report));
}

void ClockWidget::onSetDateTime()
{
    QDateTime dateTime = mDateEdit->dateTime();

    DeviceReport report(DeviceReport::SetDateTime);
    report.set("y", dateTime.date().year());
    report.set("mn", dateTime.date().month() - 1);
    report.set("md", dateTime.date().day());
    report.set("w", dateTime.date().dayOfWeek());

    report.set("h", dateTime.time().hour());
    report.set("m", dateTime.time().minute());
    report.set("s", dateTime.time().second());

    mDevice->send(report.toString());
}

QString ClockWidget::getTimeString(const DeviceReport &report) const
{
    const int hours = report.get("h", 0);
    const int minutes = report.get("m", 0);
    const int seconds = report.get("s", 0);

    return QTime(hours, minutes, seconds).toString();
}

QString ClockWidget::getDateString(const DeviceReport &report) const
{
    const int year = report.get("y", 0);
    const int month = report.get("mn", 0) + 1;
    const int monthDay = report.get("md", 0);

    return QDate(year, month, monthDay).toString(Qt::RFC2822Date);
}
