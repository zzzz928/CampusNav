#ifndef MAINWINDOW_H    //预处理符，判断是否定义过宏MAINWINDOW_H
#define MAINWINDOW_H
#include <QTextEdit>
#include <QVBoxLayout>  //Qt垂直布局控制器
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QInputDialog>
#include <QGroupBox>
#include <QCheckBox>
#include <QDialog>
#include <QMainWindow>
#include <QList>
#include <QListWidget>
#include <QComboBox>
#include "graph.h"

//引入地图控件类
class MapWidget;

//创建一个主窗口类
class CampusNavWindow : public QMainWindow {
    Q_OBJECT  // 这个宏必须在类的最开始，Qt核心宏

public:
    //创建构造函数，创建对象时调用使初始化
    explicit CampusNavWindow(QWidget *parent = nullptr);    //explicit禁止隐式类型转换
    ~CampusNavWindow() override;        // 添加析构函数声明，用于释放资源


    void refreshSpotList();     //刷新景点列表
    void refreshMap();          //刷新地图

//slots是Qt关键字，以下定义的都为槽函数，槽函数可以通过如按钮点击的信号触发
private slots:
    void onQuerySpotInfo();         //查询景点信息
    void onQueryShortestPath();     //查询最短路径
    void onQueryAllPaths();         //查询所有路径
    void onQueryNearby();           //查询附近景点
    void onAddSpot();               //添加景点
    void onDeleteSpot();            //删除景点
    void onModifyDistance();        //修改距离
    void onRefreshData();           //刷新数据
    void onSpotSelected(QListWidgetItem *item);     //景点列表项选中
    void onUpdateSpotAllInfo();     //更新景点信息
    void onAdminLogin();            //管理员登录/退出（密码验证后显示/隐藏管理员功能区）

private:
    QString getTypeString(SpotType type);       //获取类型字符串
    void setupUI();                             //设置界面
    void applyStyleSheet();                     //应用样式表

    QListWidget *spotList = nullptr;            //声明景点列表控件
    QComboBox *typeCombo = nullptr;             //声明类型下拉选择框
    MapWidget *mapWidget = nullptr;
    int selectedStart = -1;
    int selectedEnd = -1;
    QList<int> pathNodes;

    QGroupBox *adminGroup = nullptr;            //管理员功能组框（可显示/隐藏）
    QPushButton *btnAdminLogin = nullptr;       //管理员登录/退出按钮
    QPushButton *btnUpdateSpot = nullptr;       //更新景点信息按钮
    bool isAdmin = false;                       //当前是否已登录管理员
};

#endif
