#include "NetworkWidget.h"

#include "UsbDevice.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QTimer>
#include <QMessageBox>

#include "DeviceCommand.h"

NetworkWidget::NetworkWidget(UsbDevice *device, QWidget *parent) : QWidget{parent}
{
    mDevice = device;

    mUpdateTimer = new QTimer(this);
    mUpdateTimer->setInterval(std::chrono::seconds(2));

    QVBoxLayout *main_l = new QVBoxLayout(this);

    QHBoxLayout *formWrapper_l = new QHBoxLayout;
    formWrapper_l->setContentsMargins({});
    main_l->addLayout(formWrapper_l);

    QFormLayout *form_l = new QFormLayout;
    formWrapper_l->addLayout(form_l);
    formWrapper_l->addStretch();

    QPushButton *connectButton = new QPushButton(tr("Connect"));
    mStatusLabel = new QLabel(tr("Unknown"));
    mSsidEdit = new QLineEdit;
    mPassEdit = new QLineEdit;
    form_l->addRow(tr("Status:"), mStatusLabel);
    form_l->addRow(tr("SSID:"), mSsidEdit);
    form_l->addRow(tr("Password:"), mPassEdit);
    form_l->addRow(connectButton);

    connect(mDevice, &UsbDevice::opened, this, &NetworkWidget::onDeviceOpened);
    connect(mDevice, &UsbDevice::closed, this, &NetworkWidget::onDeviceClosed);
    connect(mDevice, &UsbDevice::recieved, this, &NetworkWidget::onReport);

    connect(connectButton, &QPushButton::clicked, this, &NetworkWidget::onConnectClicked);

    connect(mUpdateTimer, &QTimer::timeout, this, &NetworkWidget::requestStatus);
}

void NetworkWidget::onDeviceOpened()
{
    requestStatus();
    mUpdateTimer->start();
}

void NetworkWidget::onDeviceClosed()
{
    mUpdateTimer->stop();
    mStatusLabel->setText(tr("Unknown"));
}

void NetworkWidget::onReport(const std::string &msg)
{
    DeviceReport report(msg);

    switch (report.getCmd()) {
    case DeviceReport::NetworkState: onState(report); break;
    default: break;
    }
}

void NetworkWidget::onConnectClicked()
{
    QString ssid = mSsidEdit->text();
    QString pass = mPassEdit->text();

    if (ssid.isEmpty() || pass.isEmpty()) {
        return;
    }

    if(ssid.size() + pass.size() > 128) {
        QMessageBox::warning(
            this, tr("Info"),
            tr("Total length of SSID and password must be less then 129 symbols"));
        return;
    }

    DeviceReport report(DeviceReport::ConnectToWifi);
    report.set("s", ssid.toStdString());
    report.set("p", pass.toStdString());
    mDevice->send(report.toString());
}

void NetworkWidget::onState(const DeviceReport &report)
{
    QString statusStr;
    bool connected = report.get("s", 0);
    statusStr = connected ? tr("Connected to wifi ") : tr("Not connected to wifi");
    if (connected) {
        QString ssid = QString::fromStdString(report.get("ssid", std::string()));
        if (ssid.isEmpty()) {
            ssid = "<empty>";
        }

        statusStr += "\"" + ssid + "\"";
    }

    mStatusLabel->setText(statusStr);
}

void NetworkWidget::requestStatus()
{
    DeviceReport report(DeviceReport::GetNetworkState);
    mDevice->send(report.toString());
}
