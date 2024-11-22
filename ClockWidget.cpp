#include "ClockWidget.h"

#include <QFormLayout>
#include <QLabel>
#include <QTime>
#include <QDateTimeEdit>
#include <QPushButton>

#include "DeviceCommand.h"
#include "UsbDevice.h"


ClockWidget::ClockWidget(UsbDevice *device, QWidget *parent) : QWidget{parent}
{
    mDevice = device;

    mUpdateTimer = new QTimer(this);
    mUpdateTimer->setInterval(std::chrono::seconds(1));

    QVBoxLayout *main_l = new QVBoxLayout(this);

    QHBoxLayout *dateWrapper_l = new QHBoxLayout;
    main_l->addLayout(dateWrapper_l);
    main_l->addStretch();

    QFormLayout *dateState_l = new QFormLayout;
    dateWrapper_l->addLayout(dateState_l);
    mTimeLabel = new QLabel(tr("Unknown"));
    dateState_l->addRow(tr("Time:"), mTimeLabel);

    mDateLabel = new QLabel(tr("Unknown"));
    dateState_l->addRow(tr("Date:"), mDateLabel);

    QHBoxLayout *setDate_l = new QHBoxLayout;
    dateWrapper_l->addLayout(setDate_l);
    dateWrapper_l->addStretch();

    mDateEdit = new QDateTimeEdit;
    mDateEdit->setCalendarPopup(true);
    mDateEdit->setDateTime(QDateTime::currentDateTime());

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

    DeviceReport report(GetDateTime);
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

    switch (report.cmd) {
    case DateTimeState: onDateTime(report.data); break;
    default: break;
    }
}

void ClockWidget::onDateTime(const std::string &data)
{
    if (data.size() >= 14) {
        mTimeLabel->setText(getTimeString(data));
        mDateLabel->setText(getDateString(data));
    }
}

void ClockWidget::onSetDateTime()
{
    QDateTime dateTime = mDateEdit->dateTime();
    QString dateStr = QString::number(dateTime.date().year());
    dateStr += QString::asprintf("%02i", dateTime.date().month() - 1);
    dateStr += QString::asprintf("%02i", dateTime.date().day());
    dateStr += QString::asprintf("%i", dateTime.date().dayOfWeek());

    dateStr += QString::asprintf("%02i", dateTime.time().hour());
    dateStr += QString::asprintf("%02i", dateTime.time().minute());
    dateStr += QString::asprintf("%02i", dateTime.time().second());

    DeviceReport report(SetDateTime, dateStr.toStdString());
    mDevice->send(report.toString());
}

QString ClockWidget::getTimeString(const std::string &data) const
{
    const QString hours = QString::fromStdString(std::string(data.begin() + 8, data.begin() + 10));
    const QString min = QString::fromStdString(std::string(data.begin() + 10, data.begin() + 12));
    const QString sec = QString::fromStdString(std::string(data.begin() + 12, data.begin() + 14));

    QTime time(hours.toInt(), min.toInt(), sec.toInt());

    return time.toString();
}

QString ClockWidget::getDateString(const std::string &data) const
{
    const QString year = QString::fromStdString(std::string(data.begin(), data.begin() + 4));
    const QString month = QString::fromStdString(std::string(data.begin() + 4, data.begin() + 6));
    const QString day = QString::fromStdString(std::string(data.begin() + 6, data.begin() + 8));

    QDate date(year.toInt(), month.toInt() + 1, day.toInt());
    return date.toString();
}
