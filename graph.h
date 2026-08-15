#pragma once

#include <stdio.h>
#include <string.h>

//设整个校园最多50个景点
#define MAX 50
//无穷大标记，两个景点之间不连通
#define INF 9999

//景点类型枚举
//采用位运算设计，支持一个景点多选多种类型
//每个类型占用一个二进制位，通过 1 << n 实现位标记
//多个类型组合使用 按位或(|) 运算，判断类型使用 按位与(&) 运算
typedef enum {
    TYPE_NONE        = 0,           // 无类型（创建景点时默认）
    TYPE_GATE        = 1 << 0,      // 校门  二进制 0000000001
    TYPE_DORM        = 1 << 1,      // 宿舍  二进制 0000000010
    TYPE_MARKET      = 1 << 2,      // 超市  二进制 0000000100
    TYPE_CANTEEN     = 1 << 3,      // 食堂  二进制 0000001000
    TYPE_HOTEL       = 1 << 4,      // 酒店  二进制 0000010000
    TYPE_SPORT       = 1 << 5,      // 运动场所 二进制 0000100000
    TYPE_TEACH_BUILD = 1 << 6,      // 教学楼 二进制 0001000000
    TYPE_LIBRARY     = 1 << 7,      // 图书馆 二进制 0010000000
    TYPE_COLLEGE     = 1 << 8,      // 学院楼 二进制 0100000000
    TYPE_OTHER       = 1 << 9       // 其他  二进制 1000000000
} SpotType;

//景点结构体：存储单个景点全部信息
typedef struct {
    int id;//用于识别景点的唯一编号
    char name[50]; // 景点名称，字符数组，最大49个有效字符（留1位存结束符）
    char desc[500]; // 景点简介，字符数组，最大499个有效字符
    SpotType type; // 景点类型
    int x, y;// 景点在地图上的X、Y坐标，用于界面绘制
} Spot;

//全局变量声明，在整个项目中共享图数据
extern Spot spots[MAX];         // 景点数组，存储全部景点信息
extern int graph[MAX][MAX];     // 图邻接矩阵，存储景点之间的连通距离
extern int spotCount;           // 当前系统中已创建的景点实际数量


//初始化邻接矩阵
void initGraphData();

//根据景点名称查找数组下标
int findSpotByName(const char* name);

//迪杰斯特拉算法：求解单源最短路径
void dijkstra(int start, int end, int dist[], int prev[]);

//工具函数：位标记:分号分隔字符串
void typeToString(SpotType type, char* outStr, int maxLen);

//工具函数：用于界面多选的获取所有类型名称列表
const char** getTypeNames();