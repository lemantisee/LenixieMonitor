#include "MainWindow.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QTabWidget>
#include <QSplitter>

#include "UsbDevice.h"

#include "StatusPanel.h"
#include "LogWidget.h"
#include "ClockWidget.h"
#include "NetworkWidget.h"

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
    setCentralWidget(mainWidget);

    StatusPanel *statusWidget = new StatusPanel(mUsbDevice);
    main_l->addWidget(statusWidget);

    QSplitter *splitter = new QSplitter(Qt::Vertical);
    splitter->addWidget(buildTabWidget());
    splitter->addWidget(new LogWidget(mUsbDevice));

    main_l->addWidget(splitter);

    if (!mUsbDevice->open(vendorId, productId)) {
        LOG_ERROR("Unable to open device");
    }

    resize(640, 480);

    show();
}

QWidget *MainWindow::buildTabWidget()
{
    QTabWidget *tabWidget = new QTabWidget;

    tabWidget->addTab(new ClockWidget(mUsbDevice), tr("Clock"));
    tabWidget->addTab(new NetworkWidget(mUsbDevice), tr("Network"));

    return tabWidget;
}
