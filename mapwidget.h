#ifndef MAPWIDGET_H
#define MAPWIDGET_H
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QWidget>
#include <QList>
#include "graph.h"

//定义一个界面控件类，继承自Qt的QWidget类
class MapWidget : public QWidget {
    Q_OBJECT

public:
    //构造函数，没有返回值，函数名与类名一致，初始化对象
    explicit MapWidget(QWidget *parent = nullptr);
    //更新地图函数，传入起点、终点索引和整型路径节点列表
    void updateMap(int startIdx, int endIdx, const QList<int> &path);

protected:
    //绘制事件，重写paintEvent父类方法
    void paintEvent(QPaintEvent *event) override;

private:
    int startIdx = -1;
    int endIdx = -1;
    QList<int> pathNodes;//路径节点
};

#endif