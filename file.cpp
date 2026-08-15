//数据文件处理，负责读写数据文件
#include "file.h"
#include "graph.h"

//从景点txt文件加载景点数据
int loadSpots(const char* filename) {
    FILE* fp = fopen(filename, "r");        //只读模式打开文件
    if (!fp) {
        printf("无法打开文件 %s\n", filename);
        return 0;
    }

    spotCount = 0;       //加载前清空当前景点计数
    char line[500];      //单行读取缓冲区
    while (fgets(line, sizeof(line), fp) && spotCount < MAX) {
        int id, type, x, y;
        char name[50], desc[200];

        //清空字符数组，防止脏数据
        memset(name, 0, sizeof(name));
        memset(desc, 0, sizeof(desc));

        //sscanf按 | 分割读取6个字段，匹配成功返回6
        if (sscanf(line, "%d|%49[^|]|%199[^|]|%d|%d|%d",
                   &id, name, desc, &type, &x, &y) == 6) {
            spots[spotCount].id = id;                       //赋值到全局spots数组当前下标
            strncpy(spots[spotCount].name, name, 49);
            spots[spotCount].name[49] = '\0';               //强制补充结束符
            strncpy(spots[spotCount].desc, desc, 199);
            spots[spotCount].desc[199] = '\0';
            spots[spotCount].type = (SpotType)type;         //数字强转为位枚举
            spots[spotCount].x = x;
            spots[spotCount].y = y;
            spotCount++;//有效景点数加一
        }
    }

    fclose(fp);          //关闭文件释放资源
    printf("加载了 %d 个景点\n", spotCount);
    return 1;
}

//从邻接矩阵txt文件加载距离数据
int loadDistance(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        printf("无法打开文件 %s\n", filename);
        return 0;
    }

    //双重循环读取矩阵所有数值
    for (int i = 0; i < spotCount; i++) {
        for (int j = 0; j < spotCount; j++) {
            //读取失败则赋值INF表示无通路
            if (fscanf(fp, "%d", &graph[i][j]) != 1) {
                graph[i][j] = INF;
            }
        }
    }

    fclose(fp);
    printf("加载了 %dx%d 距离矩阵\n", spotCount, spotCount);
    return 1;
}

//保存全部数据进两个txt文件
void saveAll() {
    //保存景点信息至spots.txt
    FILE* fp = fopen("data/spots.txt", "w");    //以写入模式打开文件
    if (fp) {
        for (int i = 0; i < spotCount; i++) {
            //按分隔符|输出全部字段进文件，type直接输出数字位值，最后换行再输入下一景点
            fprintf(fp, "%d|%s|%s|%d|%d|%d\n",
                    spots[i].id, spots[i].name, spots[i].desc,
                    spots[i].type, spots[i].x, spots[i].y);
        }
        fclose(fp);
    }

    //保存邻接矩阵至distance.txt
    fp = fopen("data/distance.txt", "w");
    if (fp) {
        for (int i = 0; i < spotCount; i++) {
            for (int j = 0; j < spotCount; j++) {
                fprintf(fp, "%d ", graph[i][j]);     //格式化输出数据到文件
            }
            fprintf(fp, "\n");  //每行结束则换行
        }
        fclose(fp);
    }

    printf("数据保存完成\n");
}