//管理员模块，提供景点的增删改查功能
#include "admin.h"
#include "file.h"

// 添加新景点，处理邻接矩阵扩展、三角边冲突
void addSpot(Spot newSpot, int distances[]) {
    //判断当前景点数量达到数组上限MAX，若已达到则无法新增
    if (spotCount >= MAX) {
        printf("景点数量已达上限！\n");
        return;
    }

    // 1. 将新景点与所有旧景点邻接矩阵对应行列初始化为全部不连通(INF)
    for (int i = 0; i < spotCount; i++) {
        graph[i][spotCount] = INF;      //列初始化
        graph[spotCount][i] = INF;      //行初始化
    }
    // 规定自己到自己距离为 0
    graph[spotCount][spotCount] = 0;

    // 2. 根据distance数组（传入距离 >0 的位置）建立通路
    for (int i = 0; i < spotCount; i++) {
        //距离大于0代表两点需要连通
        if (distances[i] > 0)
        {
            //无向图双向赋值
            graph[i][spotCount] = distances[i];
            graph[spotCount][i] = distances[i];
        }
    }
    // 3. 处理三角路径冲突：新景点所连的2个旧景点之间若存在路径，则删除该2个旧景点间路径
    for (int u = 0; u < spotCount; u++) {
        for (int v = u + 1; v < spotCount; v++) {
            //判断：u、v都与新景点连通，且u≠v
            if (graph[u][spotCount] != INF && graph[v][spotCount] != INF && u != v) {
                // 断开原 u-v 边
                graph[u][v] = INF;
                graph[v][u] = INF;
                printf("已断开原边: %s ↔ %s\n", spots[u].name, spots[v].name);
            }
        }
    }
    // 4. 保存景点信息，存入景点数组
    newSpot.id = spotCount + 1;//编号从1开始，下标从0开始
    spots[spotCount] = newSpot;
    spotCount++;    //景点数量自增

    printf("景点 %s 添加成功！\n", newSpot.name);
    saveAll();  //自动持久化写入txt文件
}

//删除景点
void delSpot(int id) {
    //下标合法性校验，超出范围直接返回
    if (id < 0 || id >= spotCount) {
        printf("景点不存在！\n");
        return;
    }

    //1. 压缩景点数组：用后序元素全部前移覆盖来删除位置
    for (int i = id; i < spotCount - 1; i++) {
        spots[i] = spots[i + 1];
    }

    //2. 压缩邻接矩阵：后续行列迁移覆盖指定行列，实现删除对应行、对应列
    for (int i = id; i < spotCount - 1; i++)//换行列移动循环
        {
        for (int j = 0; j < spotCount; j++)//换单个二维数组值移动循环
            {
            graph[i][j] = graph[i + 1][j];      //行前移
            graph[j][i] = graph[j][i + 1];      //列前移
        }
    }

    spotCount--;        //景点数量减一
    printf("景点删除成功！\n");
    saveAll();          //自动保存修改后数据
}

// 修改或删除u和v两点之间的道路距离
// newDist = INF 时，等价于删除路径
void modifyDistance(int u, int v, int newDist) {
    //景点下标无效或路径两端自己连自己则返回
    if (u < 0 || u >= spotCount || v < 0 || v >= spotCount || u == v) {
        printf("景点不存在！\n");
        return;
    }
    //无向图双向赋值
    graph[u][v] = newDist;
    graph[v][u] = newDist;
    printf("路径修改成功！%s ↔ %s 距离 = %d\n", spots[u].name, spots[v].name, newDist);
    saveAll(); // 自动保存到文件
}

//更新景点信息（名称、简介、类型）
void updateSpotInfo(int id, char* newName, char* newDesc, SpotType newType) {
    if (id < 0 || id >= spotCount) {
        printf("景点不存在！\n");
        return;
    }
    //传入非NULL则覆盖原有名称
    if (newName != NULL) strcpy(spots[id].name, newName);
    //传入非NULL则覆盖原有简介
    if (newDesc != NULL) strcpy(spots[id].desc, newDesc);
    spots[id].type = newType;     //覆盖多类型位标记
    printf("景点信息更新成功！\n");
    saveAll();
}

//清除指定景点所有连接通路（用于管理员更新景点信息功能）
void clearAllEdgeOfSpot(int id)
{
    if (id < 0 || id >= spotCount)  //检验下标合法性
        return;
    // 遍历所有顶点
    for (int i = 0; i < spotCount; i++)
    {
        if (i != id)    //与下标为id所在行列双向置为INF，即删除该景点所连所有边
        {
            graph[id][i] = INF;
            graph[i][id] = INF;
        }
    }
    printf("已清空该景点所有原有连通路径\n");
    saveAll();
}