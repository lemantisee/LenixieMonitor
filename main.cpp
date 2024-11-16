#include <QApplication>

#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    new MainWindow;

    // DeviceLogger devLogger;

    // if (!devLogger.init(&usbDevice)) {
    //     LOG_ERROR("Unable to init device logger client");
    //     return 1;
    // }

    // while (true) {
    //     devLogger.process();
    // }

    return a.exec();
}
