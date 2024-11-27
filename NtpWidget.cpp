#include "NtpWidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPushButton>

#include "UsbDevice.h"
#include "DeviceReport.h"

#include "Logger.h"

NtpWidget::NtpWidget(UsbDevice *device, QWidget *parent) : QWidget{parent}
{
    mDevice = device;

    QVBoxLayout *main_l = new QVBoxLayout(this);

    QHBoxLayout *formWrapper_l = new QHBoxLayout;
    formWrapper_l->setContentsMargins({});
    main_l->addLayout(formWrapper_l);

    QFormLayout *form_l = new QFormLayout;
    form_l->setContentsMargins({});
    formWrapper_l->addLayout(form_l);
    formWrapper_l->addStretch();

    mTimeZoneEdit = new QSpinBox;
    mTimeZoneEdit->setRange(-12, 12);
    form_l->addRow(tr("Time zone:"), mTimeZoneEdit);

    mServerEdit = new QLineEdit;
    QPushButton *setServerButton = new QPushButton(tr("Set"));
    QHBoxLayout *serverField_l = new QHBoxLayout;
    serverField_l->setContentsMargins({});
    serverField_l->addWidget(mServerEdit);
    serverField_l->addWidget(setServerButton);
    serverField_l->addStretch();

    form_l->addRow(tr("NTP server:"), serverField_l);

    QPushButton *syncTimeButton = new QPushButton(tr("Sync time by NTP"));
    QHBoxLayout *sync_l = new QHBoxLayout;
    sync_l->setContentsMargins({});
    sync_l->addWidget(syncTimeButton);
    sync_l->addStretch();

    main_l->addLayout(sync_l);
    main_l->addStretch();


    connect(mDevice, &UsbDevice::opened, this, &NtpWidget::onDeviceOpened);
    connect(mDevice, &UsbDevice::closed, this, &NtpWidget::onDeviceClosed);
    connect(mDevice, &UsbDevice::recieved, this, &NtpWidget::onReport);

    connect(mTimeZoneEdit, &QSpinBox::valueChanged, this, &NtpWidget::onTimezoneChanged);
    connect(setServerButton, &QPushButton::clicked, this, &NtpWidget::setServer);
    connect(syncTimeButton, &QPushButton::clicked, this, &NtpWidget::syncNtpTime);

}

void NtpWidget::onDeviceOpened()
{
    mDevice->send(DeviceReport(DeviceReport::GetNtpState).toString());
}

void NtpWidget::onDeviceClosed()
{

}

void NtpWidget::onReport(const std::string &msg)
{
    DeviceReport report(msg);

    if (report.getCmd() != DeviceReport::NtpState) {
        return;
    }

    QString url = QString::fromStdString(report.get("url", std::string()));
    int timezone = report.get("t", 15);
    mServerEdit->setText(url);

    if (timezone > 12 || timezone < -12) {
        LOG_ERROR("Time zone value out of range: %i", timezone);
        return;
    }

    mTimeZoneEdit->blockSignals(true);
    mTimeZoneEdit->setValue(timezone);
    mTimeZoneEdit->blockSignals(false);
}

void NtpWidget::onTimezoneChanged(int timezone)
{
    DeviceReport report(DeviceReport::SetTimezone);
    report.set("t", timezone);
    mDevice->send(report.toString());

    mDevice->send(DeviceReport(DeviceReport::GetNtpState).toString());
}

void NtpWidget::setServer()
{
    QString serverUrl = mServerEdit->text();
    if (serverUrl.isEmpty()) {
        return;
    }

    DeviceReport report(DeviceReport::SetNtpServer);
    report.set("url", serverUrl.toStdString());
    mDevice->send(report.toString());

    mDevice->send(DeviceReport(DeviceReport::GetNtpState).toString());
}

void NtpWidget::syncNtpTime()
{
    mDevice->send(DeviceReport(DeviceReport::SyncNtpTime).toString());
}
