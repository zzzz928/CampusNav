//主程序入口
#include <QApplication>
#include <QMessageBox>
#include <QFont>

#ifdef _WIN32
#include <windows.h>
#endif

#include "graph.h"
#include "file.h"
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    #endif

    //创建Qt应用程序实例
    QApplication app(argc,argv);
    app.setFont(QFont("Microsoft YaHei", 10));

    //初始化邻接矩阵
    initGraphData();

    //加载景点数据，如果返回false则说明加载失败，弹出对话框并返回1表示程序异常退出
    if (!loadSpots("data/spots.txt")) {
        QMessageBox::critical(nullptr, "错误", "无法加载景点数据！\n请确保 data/spots.txt 文件存在");
        return 1;
    }

    //加载距离数据
    if (!loadDistance("data/distance.txt")) {
        QMessageBox::critical(nullptr, "错误", "无法加载距离数据！\n请确保 data/distance.txt 文件存在");
        return 1;
    }
    //创建主窗口对象window
    CampusNavWindow window;
    //调用show方法显示窗口
    window.show();

    //运行应用程序，进入事件循环，持续运行直到退出
    return app.exec();
}