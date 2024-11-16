#pragma once

#include <QWidget>

#include <QPlainTextEdit>

class UsbDevice;

class LogWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LogWidget(UsbDevice *device, QWidget *parent = nullptr);

private:
    void onLog(const std::string &log);

    QPlainTextEdit *mTextEdit = nullptr;
};
