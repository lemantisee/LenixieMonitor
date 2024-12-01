#include "StatusPanel.h"

#include <QHBoxLayout>
#include <QLabel>

#include "UsbDevice.h"
#include "DeviceReport.h"

StatusPanel::StatusPanel(UsbDevice *device, QWidget *parent) : QWidget{parent}
{
    mDevice = device;

    QHBoxLayout *main_l = new QHBoxLayout(this);

    mFirmwareVersionLabel = new QLabel();
    mStatusLabel = new QLabel(tr("Disconnected"));
    main_l->addWidget(mStatusLabel);
    main_l->addStretch();
    main_l->addWidget(mFirmwareVersionLabel);

    connect(device, &UsbDevice::opened, this, &StatusPanel::onOpen);
    connect(device, &UsbDevice::closed, this, [this] {
        mStatusLabel->setText(tr("Disconnected"));
        mFirmwareVersionLabel->clear();
    });
    connect(device, &UsbDevice::recieved, this, &StatusPanel::onReport);
}

void StatusPanel::onOpen()
{
    mStatusLabel->setText(tr("Connected"));

    DeviceReport report(DeviceReport::GetVersion);
    mDevice->send(report.toString());
}

void StatusPanel::onReport(const std::string &msg)
{
    DeviceReport report(msg);

    if (report.getCmd() != DeviceReport::VersionInfo) {
        return;
    }

    std::string version = report.get("f", std::string());
    QString versionStr = "Firmware ver: " + QString::fromStdString(version);

    mFirmwareVersionLabel->setText(versionStr);
}
