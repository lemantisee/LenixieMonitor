#pragma once

#include <QWidget>

class UsbDevice;

class StatusPanel : public QWidget
{
    Q_OBJECT
public:
    explicit StatusPanel(UsbDevice *device, QWidget *parent = nullptr);

};
