#pragma once

int loadSpots(const char* filename);    //从景点txt文件加载景点数据
int loadDistance(const char* filename); //从邻接矩阵txt文件加载距离数据
void saveAll();                         //保存全部数据进两个txt文件