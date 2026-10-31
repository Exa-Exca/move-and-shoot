#include<Windows.h>
#include <timeapi.h>
#pragma comment(lib, "winmm.lib")
#include "MainWindow.h"
#include <QtWidgets/QApplication>


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    ::timeBeginPeriod(1); // 将 Windows 的系统计时器精度强制锁定在 1 毫秒
    MainWindow window;
    window.show();
    return app.exec();
}
