//地图可视化模块，负责绘制地图
#include "mapwidget.h"

//实现构造函数，利用构造函数初始化地图对象
MapWidget::MapWidget(QWidget *parent) : QWidget(parent)
{
    setMouseTracking(true);//鼠标追踪
    setMinimumSize(1000, 700); //设置初始窗口大小
}

/**
 * @brief 更新地图数据并触发重绘
 * @param startIdx 路径起点下标
 * @param endIdx 路径终点下标
 * @param path 高亮路径节点数组
 */
void MapWidget::updateMap(int startIdx, int endIdx, const QList<int> &path)
{
    this->startIdx = startIdx;
    this->endIdx = endIdx;
    this->pathNodes = path;
    update(); // 调用Qt自带的重绘事件函数
}

/**
 * @brief 绘制事件：绘制地图、连线、景点节点、高亮路径
 */
void MapWidget::paintEvent(QPaintEvent *event)
{
    //创建QPainter对象，并绑定到当前控件this
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing); // 抗锯齿，让绘制的图形边缘更平滑
    painter.fillRect(rect(), QColor(232, 245, 233)); // 用fillRect来指定颜色填充整体为浅绿色背景

    // 无景点时提示文字
    if (spotCount == 0)
    {
        painter.drawText(rect(), Qt::AlignCenter, "暂无景点数据");//在指定区域绘制文字并居中对齐
        return;
    }

    // 1. 计算所有景点坐标极值，用于自适应缩放
    int minX = 10000, minY = 10000;
    int maxX = -10000, maxY = -10000;
    for (int i = 0; i < spotCount; i++)
    {
        if (spots[i].x < minX) minX = spots[i].x;
        if (spots[i].y < minY) minY = spots[i].y;
        if (spots[i].x > maxX) maxX = spots[i].x;
        if (spots[i].y > maxY) maxY = spots[i].y;
    }

    //计算地图的高度和宽度范围
    int rangeX = maxX - minX;
    int rangeY = maxY - minY;
    if (rangeX == 0) rangeX = 1;
    if (rangeY == 0) rangeY = 1;

    // 计算缩放比例
    double scale = qMin((width() - 120) / (double)rangeX, (height() - 120) / (double)rangeY);
    //计算偏移量，让地图居中显示
    int offsetX = (width() - (rangeX * scale)) / 2;
    int offsetY = (height() - (rangeY * scale)) / 2;

    // 2. 绘制所有普通连通边（灰色线条 + 距离文字）
    QPen edgePen(QColor(100, 100, 100), 2);
    painter.setPen(edgePen);//应用画笔重置
    for (int i = 0; i < spotCount; i++)
    {
        for (int j = i + 1; j < spotCount; j++)
        {
            // 判断两个景点之间是否有通路
            if (graph[i][j] != INF && graph[i][j] > 0)
            {
                //坐标转换：将逻辑坐标转换为屏幕坐标，spots[i].x - minX保持相对距离保持不变
                int x1 = offsetX + (spots[i].x - minX) * scale;
                int y1 = offsetY + (spots[i].y - minY) * scale;
                int x2 = offsetX + (spots[j].x - minX) * scale;
                int y2 = offsetY + (spots[j].y - minY) * scale;

                painter.drawLine(x1, y1, x2, y2);
                // 在连线中点绘制距离
                int mx = (x1 + x2) / 2;
                int my = (y1 + y2) / 2;
                //在连线中点居中显示显示距离数值，并将整数转换为字符
                painter.drawText(mx - 10, my - 5, QString::number(graph[i][j]));
            }
        }
    }

    // 3. 绘制高亮路径（绿色粗线，代表查询出的最短路径）
    if (pathNodes.size() > 1)
    {
        QPen pathPen(QColor(76, 175, 80), 5);
        painter.setPen(pathPen);
        for (int k = 0; k < pathNodes.size() - 1; k++)
        {
            //遍历路径节点列表，依次取出相邻的两个节点
            int i = pathNodes[k];
            int j = pathNodes[k + 1];
            int x1 = offsetX + (spots[i].x - minX) * scale;
            int y1 = offsetY + (spots[i].y - minY) * scale;
            int x2 = offsetX + (spots[j].x - minX) * scale;
            int y2 = offsetY + (spots[j].y - minY) * scale;

            painter.drawLine(x1, y1, x2, y2);
        }
    }

    // 4. 逐个绘制景点节点（圆形 + 编号 + 名称 + 按类型区分颜色）
    for (int i = 0; i < spotCount; i++)
    {
        int x = offsetX + (spots[i].x - minX) * scale;
        int y = offsetY + (spots[i].y - minY) * scale;
        QColor color;

        // 起点、终点、路径节点 优先配色
        if (i == startIdx && startIdx == endIdx && startIdx != -1)
        {
            color = QColor(255, 215, 0);   // 金黄色：起点终点为同一个
        }
        else if (i == startIdx)
        {
            color = QColor(50, 205, 50);   // 绿色：起点
        }
        else if (i == endIdx)
        {
            color = QColor(255, 69, 0);    // 橙红：终点
        }
        else if (pathNodes.contains(i))
        {
            color = QColor(76, 175, 90);   // 深绿：路径途经点
        }
        else
        {
            // 根据景点类型设置节点颜色（支持多类型，按优先级取第一个匹配）
            if (spots[i].type & TYPE_GATE)          color = QColor(128, 128, 128);  // 校门-灰色
            else if (spots[i].type & TYPE_DORM)     color = QColor(255, 105, 180);  // 宿舍-粉色
            else if (spots[i].type & TYPE_MARKET)   color = QColor(0, 191, 255);    // 超市-天蓝
            else if (spots[i].type & TYPE_CANTEEN)  color = QColor(255, 165, 0);    // 食堂-橙色
            else if (spots[i].type & TYPE_HOTEL)    color = QColor(138, 43, 226);   // 酒店-紫蓝
            else if (spots[i].type & TYPE_SPORT)    color = QColor(34, 139, 34);    // 运动场所-深绿
            else if (spots[i].type & TYPE_TEACH_BUILD) color = QColor(210, 105, 30);// 教学楼-棕
            else if (spots[i].type & TYPE_LIBRARY)  color = QColor(100, 149, 237);  // 图书馆-浅蓝
            else if (spots[i].type & TYPE_COLLEGE)  color = QColor(220, 20, 60);    // 学院楼-大红
            else if (spots[i].type & TYPE_OTHER)    color = QColor(20, 128, 144);  // 其他-藏青
            else                                    color = QColor(70, 130, 180);  // 默认-钢蓝
        }

        // 绘制圆形节点
        painter.setBrush(QBrush(color));
        painter.setPen(QPen(Qt::white, 3));//设置边框
        painter.drawEllipse(QPoint(x, y), 15, 15);//绘制圆形

        painter.setPen(QPen(Qt::black, 2));
        painter.drawEllipse(QPoint(x, y), 14, 14);

        // 绘制编号与名称文字
        painter.setPen(Qt::black);
        painter.drawText(x - 5, y + 5, QString::number(spots[i].id));
        painter.drawText(x - 40, y + 28, QString::fromUtf8(spots[i].name));
    }
}