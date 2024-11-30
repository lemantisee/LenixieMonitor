#include "NetworkWidget.h"

#include "UsbDevice.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QTimer>
#include <QMessageBox>
#include <QMenu>

#include "ConnectDialog.h"
#include "DeviceReport.h"
#include "Logger.h"

NetworkWidget::NetworkWidget(UsbDevice *device, QWidget *parent) : QWidget{parent}
{
    mDevice = device;

    mUpdateTimer = new QTimer(this);
    mUpdateTimer->setInterval(std::chrono::seconds(2));

    mConnectAction = new QAction(tr("Connect"), this);
    mConnectToLastAction = new QAction(tr("Connect to last network"), this);
    mDisconnectAction = new QAction(tr("Disconnect"), this);
    mForgetAction = new QAction(tr("Forget network"), this);

    QMenu *menu = new QMenu(this);
    menu->addAction(mConnectAction);
    menu->addAction(mConnectToLastAction);
    menu->addAction(mDisconnectAction);
    menu->addAction(mForgetAction);

    mStatusLabel = new QLabel(tr("Unknown"));
    mMenuButton = new QToolButton;
    mMenuButton->setText(tr("Actions"));
    mMenuButton->setPopupMode(QToolButton::InstantPopup);
    mMenuButton->setMenu(menu);
    mMenuButton->setEnabled(false);

    QVBoxLayout *main_l = new QVBoxLayout(this);
    main_l->addWidget(mStatusLabel);
    main_l->addWidget(mMenuButton);
    main_l->addStretch();


    connect(mDevice, &UsbDevice::opened, this, &NetworkWidget::onDeviceOpened);
    connect(mDevice, &UsbDevice::closed, this, &NetworkWidget::onDeviceClosed);
    connect(mDevice, &UsbDevice::recieved, this, &NetworkWidget::onReport);

    connect(mUpdateTimer, &QTimer::timeout, this, &NetworkWidget::requestStatus);

    connect(mConnectAction, &QAction::triggered, this, &NetworkWidget::onConnectAction);
    connect(mConnectToLastAction, &QAction::triggered, this, &NetworkWidget::onConnectLastAction);
    connect(mDisconnectAction, &QAction::triggered, this, &NetworkWidget::onDisconnectAction);
    connect(mForgetAction, &QAction::triggered, this, &NetworkWidget::onForgetAction);
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

void NetworkWidget::onConnectAction()
{
    ConnectDialog dialog(this);

    QDialog::DialogCode res = QDialog::DialogCode(dialog.exec());
    if (res == QDialog::Rejected) {
        return;
    }

    const QString ssid = dialog.getSsid();
    const QString pass = dialog.getPassword();

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

    mStatusLabel->setText(tr("Connecting..."));
    mMenuButton->setEnabled(false);
}

void NetworkWidget::onConnectLastAction()
{
    DeviceReport report(DeviceReport::ConnectToLastWifi);
    mDevice->send(report.toString());
}

void NetworkWidget::onDisconnectAction()
{
    DeviceReport report(DeviceReport::DisconnectWifi);
    mDevice->send(report.toString());
}

void NetworkWidget::onForgetAction()
{
    DeviceReport report(DeviceReport::ConnectToWifi);
    report.set("s", "");
    report.set("p", "");

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

    mDisconnectAction->setEnabled(connected);
    mForgetAction->setEnabled(connected);

    mStatusLabel->setText(statusStr);
    mMenuButton->setEnabled(true);
}

void NetworkWidget::requestStatus()
{
    DeviceReport report(DeviceReport::GetNetworkState);
    mDevice->send(report.toString());
}
