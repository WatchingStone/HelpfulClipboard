#include "MainController.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);     // 创建应用程序对象，有且仅有1个
    // Widget w;                       // 创建主窗口对象
    // w.show();                       // 主窗口对象显示窗口，基于业务逻辑，不直接显示完整对象，而是显示悬浮球
    MainController controller;
    controller.start();
    
    return a.exec();                // 启动事件循环
}
