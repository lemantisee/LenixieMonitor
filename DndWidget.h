#pragma once

#include <QWidget>

#include <QSpinBox>
#include <QCheckBox>

class UsbDevice;

class DndWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DndWidget(UsbDevice *device, QWidget *parent = nullptr);

private:
    void onDeviceOpened();
    void onReport(const std::string &msg);
    void onHoursChanged(int value);
    void onEnableDndChanged(int value);

    QCheckBox *mEnableDndBox = nullptr;
    QSpinBox *mStartDndSpin = nullptr;
    QSpinBox *mEndDndSpin = nullptr;
    UsbDevice *mDevice = nullptr;
};
