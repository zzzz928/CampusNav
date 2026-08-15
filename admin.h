#pragma once
#include "graph.h"

void addSpot(Spot newSpot, int distances[]);            // 添加新景点，处理邻接矩阵扩展、三角边冲突
void delSpot(int id);                                   //删除景点
void modifyDistance(int u, int v, int newDist);         // 修改或删除u和v两点之间的道路距离
void updateSpotInfo(int id, char* newName, char* newDesc, SpotType newType);//更新景点信息（名称、简介、类型）
void clearAllEdgeOfSpot(int id);                        //清除指定景点所有连接通路（用于管理员更新景点信息功能）
