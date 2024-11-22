#include "StatusPanel.h"

#include <QHBoxLayout>
#include <QLabel>

#include "UsbDevice.h"

StatusPanel::StatusPanel(UsbDevice *device, QWidget *parent) : QWidget{parent}
{
    QHBoxLayout *main_l = new QHBoxLayout(this);

    QLabel *label = new QLabel(tr("Disconnected"));
    main_l->addWidget(label);
    main_l->addStretch();

    connect(device, &UsbDevice::opened, this, [label] { label->setText(tr("Connected")); });
    connect(device, &UsbDevice::closed, this, [label] { label->setText(tr("Disconnected")); });
}
