#include "StatusPanel.h"

#include <QHBoxLayout>
#include <QLabel>

#include "UsbDevice.h"
#include "DeviceReport.h"
#include "Logger.h"

StatusPanel::StatusPanel(UsbDevice *device, QWidget *parent) : QWidget{parent}
{
    mDevice = device;

    mFirmwareVersionLabel = new QLabel;
    mAtVersion = new QLabel;
    mSdkVersion = new QLabel;

    QVBoxLayout *ver_l = new QVBoxLayout;
    ver_l->setContentsMargins({});
    ver_l->addWidget(mFirmwareVersionLabel);
    ver_l->addWidget(mAtVersion);
    ver_l->addWidget(mSdkVersion);
    ver_l->addStretch();

    mStatusLabel = new QLabel(tr("Disconnected"));

    QVBoxLayout *stat_l = new QVBoxLayout;
    stat_l->setContentsMargins({});
    stat_l->addWidget(mStatusLabel);
    stat_l->addStretch();

    QHBoxLayout *main_l = new QHBoxLayout(this);
    main_l->addLayout(stat_l);
    main_l->addStretch();
    main_l->addLayout(ver_l);

    connect(device, &UsbDevice::opened, this, &StatusPanel::onOpen);
    connect(device, &UsbDevice::closed, this, [this] {
        mStatusLabel->setText(tr("Disconnected"));
        mFirmwareVersionLabel->hide();
        mAtVersion->hide();
        mSdkVersion->hide();
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
    QString versionStr = tr("Firmware ver: ") + QString::fromStdString(version);

    mFirmwareVersionLabel->show();
    mFirmwareVersionLabel->setText(versionStr);

    std::string atVersion = report.get("at", std::string());
    atVersion = atVersion.empty() ? "unknown" : atVersion;
    QString atVersionStr = tr("ESP AT ver: ") + QString::fromStdString(atVersion);
    mAtVersion->show();
    mAtVersion->setText(atVersionStr);

    std::string sdkVersion = report.get("sdk", std::string());
    sdkVersion = sdkVersion.empty() ? "unknown" : sdkVersion;
    QString sdkVersionStr = tr("ESP SDK ver: ") + QString::fromStdString(sdkVersion);
    mSdkVersion->show();
    mSdkVersion->setText(sdkVersionStr);
}
