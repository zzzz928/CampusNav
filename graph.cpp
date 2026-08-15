//核心算法模块，实现图论数据结构和算法实现
#include "graph.h"

//全局变量定义，对应graph.h中的extern声明
Spot spots[MAX];                                // 存储所有景点信息的数组
int graph[MAX][MAX];                            // 无向图邻接矩阵，记录景点间距离
int spotCount = 0;                              // 当前实际景点数量，初始为0

//初始化邻接矩阵
void initGraphData() {
    //遍历整个矩阵，自己到自己距离置0，其余全部置为不可达INF
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            // 对角线元素即自身距离为0，其余为无穷大
            graph[i][j] = (i == j) ? 0 : INF;
        }
    }
}

//根据景点名称匹配下标，输入：name目标景点名称（人机交互桥梁）
int findSpotByName(const char* name) {
    //遍历所有已存在的景点
    for (int i = 0; i < spotCount; i++) {
        //字符串比较，名称一致则返回当前下标
        if (strcmp(spots[i].name, name) == 0) {
            //匹配成功返回下标
            return i;
        }
    }
    //失败返回-1
    return -1;
}

//Dijkstra最短路径算法，输入：起点下标，终点下标，最短距离数组，前驱数组
void dijkstra(int start,int end, int dist[], int prev[]) {
    // 标记节点是否已确定最短路径，0=未访问，1=已访问
    int visited[MAX] = {0};

    // 初始化距离数组和前驱数组
    for (int i = 0; i < spotCount; i++) {
        // 初始距离 = 起点到当前节点的直接距离
        dist[i] = graph[start][i];
        // 有通路且不是自身，记录前驱为起点，否则前驱置-1
        prev[i] = (graph[start][i] != INF && i != start) ? start : -1;
    }
    visited[start] = 1;//起点标记为已访问
    dist[start] = 0;//起点到自身距离为0

    // 循环处理剩余spotCount-1个节点
    for (int count = 0; count < spotCount - 1; count++) {
        // 查找当前未访问节点中距离最小的节点u
        int minDist = INF, u = -1;
        for (int i = 0; i < spotCount; i++) {
            if (!visited[i] && dist[i] < minDist) {
                minDist = dist[i];
                u = i;
            }
        }

        if (u == -1) break;// 没有可达节点，提前退出循环
        visited[u] = 1;// 标记该节点最短路径已确定

        // 松弛操作：更新经过节点u的更短路径，此时dist[v]更新
        for (int v = 0; v < spotCount; v++)
        {
            // 节点v未访问、u和v连通、新路径距离更短
            if (!visited[v] && graph[u][v] != INF &&
                dist[u] + graph[u][v] < dist[v])
            {
                dist[v] = dist[u] + graph[u][v];// 更新最短距离
                prev[v] = u;// 更新前驱节点
            }
        }
    }
}


/**
 * @brief 位标记 → 分号分隔类型字符串
 * 例：TYPE_GATE|TYPE_DORM → "校门;宿舍"
 * @param type 位标记类型
 * @param outStr 输出缓冲区
 * @param maxLen 缓冲区最大长度
 */
void typeToString(SpotType type, char* outStr, int maxLen) {
    memset(outStr, 0, maxLen);//清空输出缓冲区
    char temp[128] = {0};//临时拼接字符串

    //按位判断当前景点包含的类型，拼接字符串
    if (type & TYPE_GATE)      strcat(temp, "校门;");
    if (type & TYPE_DORM)      strcat(temp, "宿舍;");
    if (type & TYPE_MARKET)    strcat(temp, "超市;");
    if (type & TYPE_CANTEEN)   strcat(temp, "食堂;");
    if (type & TYPE_HOTEL)     strcat(temp, "酒店;");
    if (type & TYPE_SPORT)     strcat(temp, "运动场所;");
    if (type & TYPE_TEACH_BUILD) strcat(temp, "教学楼;");
    if (type & TYPE_LIBRARY)   strcat(temp, "图书馆;");
    if (type & TYPE_COLLEGE)   strcat(temp, "学院楼;");
    if (type & TYPE_OTHER)     strcat(temp, "其他;");

    // 移除末尾多余的分号
    int len = strlen(temp);
    if (len > 0 && temp[len - 1] == ';') {
        temp[len - 1] = '\0';
    }
    //结果拷贝到输出缓冲区
    strncpy(outStr, temp, maxLen - 1);
}

/**
 * @brief 获取所有类型名称数组（界面多选使用）
 * @return 静态字符串数组
 */
const char** getTypeNames() {
    static const char* names[] = {
        "校门", "宿舍", "超市", "食堂", "酒店",
        "运动场所", "教学楼", "图书馆", "学院楼", "其他",
        nullptr // 数组结束标记
    };
    return names;
}
