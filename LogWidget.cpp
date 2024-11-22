#include "LogWidget.h"

#include <QVBoxLayout>
#include <QPushButton>

#include "DeviceLogger.h"

LogWidget::LogWidget(UsbDevice *device, QWidget *parent) : QWidget{parent}
{
    QVBoxLayout *main_l = new QVBoxLayout(this);

    QPushButton *clearButton = new QPushButton(tr("Clear"));
    QHBoxLayout *clearButton_l = new QHBoxLayout;
    clearButton_l->addStretch();
    clearButton_l->addWidget(clearButton);
    main_l->addLayout(clearButton_l);

    mTextEdit = new QPlainTextEdit;
    mTextEdit->setReadOnly(true);
    main_l->addWidget(mTextEdit);

    DeviceLogger *devLogger = new DeviceLogger(device, this);

    connect(clearButton, &QPushButton::clicked, this, [this] { mTextEdit->clear(); });
    connect(devLogger, &DeviceLogger::logRecived, this, &LogWidget::onLog);
}

void LogWidget::onLog(const std::string &log)
{
    mTextEdit->appendPlainText(QString::fromStdString(log));
}
