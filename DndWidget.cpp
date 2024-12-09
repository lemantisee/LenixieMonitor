#include "DndWidget.h"

#include <QVBoxLayout>
#include <QFormLayout>

#include "UsbDevice.h"
#include "DeviceReport.h"


DndWidget::DndWidget(UsbDevice *device, QWidget *parent)
    :QWidget(parent)
{
    mDevice = device;

    mEnableDndBox = new QCheckBox(tr("Enable DND"));
    mEnableDndBox->setChecked(false);

    mStartDndSpin = new QSpinBox;
    mStartDndSpin->setRange(0, 23);

    mEndDndSpin = new QSpinBox;
    mEndDndSpin->setRange(0, 23);

    QVBoxLayout *main_l = new QVBoxLayout(this);
    main_l->addWidget(mEnableDndBox);

    QHBoxLayout *hourWrapper_l = new QHBoxLayout;
    hourWrapper_l->setContentsMargins({});
    main_l->addLayout(hourWrapper_l);

    QFormLayout *hour_l = new QFormLayout;
    hour_l->addRow(tr("Start DND:"), mStartDndSpin);
    hour_l->addRow(tr("End DND:"), mEndDndSpin);
    hourWrapper_l->addLayout(hour_l);
    hourWrapper_l->addStretch();

    connect(mEnableDndBox, &QCheckBox::stateChanged, this, &DndWidget::onEnableDndChanged);
    connect(mStartDndSpin, &QSpinBox::valueChanged, this, &DndWidget::onHoursChanged);
    connect(mEndDndSpin, &QSpinBox::valueChanged, this, &DndWidget::onHoursChanged);

    connect(mDevice, &UsbDevice::opened, this, &DndWidget::onDeviceOpened);
    connect(mDevice, &UsbDevice::recieved, this, &DndWidget::onReport);
}

void DndWidget::onDeviceOpened()
{
    mDevice->send(DeviceReport(DeviceReport::GetDndState).toString());
}

void DndWidget::onReport(const std::string &msg)
{
    DeviceReport report(msg);

    if (report.getCmd() != DeviceReport::DndState) {
        return;
    }


    bool enabledDnd = report.get("s", 0);
    uint32_t startH = report.get("sh", 25);
    uint32_t endH = report.get("eh", 25);

    mEnableDndBox->blockSignals(true);
    mEnableDndBox->setChecked(enabledDnd);
    mEnableDndBox->blockSignals(false);

    mStartDndSpin->blockSignals(true);
    mStartDndSpin->setValue(startH);
    mStartDndSpin->blockSignals(false);

    mEndDndSpin->blockSignals(true);
    mEndDndSpin->setValue(endH);
    mEndDndSpin->blockSignals(false);
}

void DndWidget::onHoursChanged(int value)
{
    DeviceReport report(DeviceReport::SetDndHours);
    report.set("sh", mStartDndSpin->value());
    report.set("eh", mEndDndSpin->value());

    mDevice->send(report.toString());
    mDevice->send(DeviceReport(DeviceReport::GetDndState).toString());
}

void DndWidget::onEnableDndChanged(int value)
{
    DeviceReport report(DeviceReport::EnableDnd);
    report.set("s", mEnableDndBox->isChecked());

    mDevice->send(report.toString());
    mDevice->send(DeviceReport(DeviceReport::GetDndState).toString());
}
