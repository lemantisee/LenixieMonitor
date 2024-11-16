#include "MainWindow.h"

#include <QVBoxLayout>
#include <QLabel>

#include "UsbDevice.h"
#include "LogWidget.h"

#include "Logger.h"

namespace {
const uint32_t vendorId = 0x0483;
const uint32_t productId = 0x5750;

} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow{parent}
{
    mUsbDevice = new UsbDevice(this);

    QWidget *mainWidget = new QWidget;
    QVBoxLayout *main_l = new QVBoxLayout(mainWidget);
    mainWidget->setLayout(main_l);

    mLabel = new QLabel("Not connected");
    main_l->addWidget(mLabel);

    LogWidget *logWidget = new LogWidget(mUsbDevice);

    main_l->addWidget(logWidget);

    setCentralWidget(mainWidget);

    connect(mUsbDevice, &UsbDevice::opened, this, &MainWindow::onDeviceOpened);
    connect(mUsbDevice, &UsbDevice::closed, this, &MainWindow::onDeviceClosed);

    if (!mUsbDevice->open(vendorId, productId)) {
        LOG_ERROR("Unable to open device");
    }

    resize(640, 480);

    show();
}

void MainWindow::onDeviceOpened()
{
    mLabel->setText("Connected");
}

void MainWindow::onDeviceClosed()
{
    mLabel->setText("Not connected");
}
