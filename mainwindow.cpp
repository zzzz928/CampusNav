//图形界面主窗口模块，实现用户交互和业务逻辑
#include "mainwindow.h"
#include "mapwidget.h"
#include "graph.h"
#include "file.h"
#include "admin.h"

//构造函数
CampusNavWindow::CampusNavWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("校园导航系统");
    resize(1400, 900);
    setupUI();          // 搭建UI布局
    applyStyleSheet();  // 加载样式
    refreshSpotList();  // 刷新景点列表
    refreshMap();       // 刷新地图
}

//析构函数
CampusNavWindow::~CampusNavWindow() {}

//设置界面
void CampusNavWindow::setupUI() {
    //创建中央控件
    QWidget *central = new QWidget(this);
    //设置主窗口的中央控件
    setCentralWidget(central);
    //创建水平布局管理器并绑定
    QHBoxLayout *mainLayout = new QHBoxLayout(central);

    //绘制左侧面板
    QWidget *leftPanel = new QWidget();
    leftPanel->setFixedWidth(280);      //设置固定宽度
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);   //创建垂直布局管理器并绑定

    //添加标题
    QLabel *titleLabel = new QLabel("校园导航系统");
    titleLabel->setStyleSheet("font-size: 22px; font-weight: bold; padding: 10px;");
    titleLabel->setAlignment(Qt::AlignCenter);//设置文字居中
    leftLayout->addWidget(titleLabel);

    //景点列表标题
    leftLayout->addWidget(new QLabel(QString("景点列表 (共%1个):").arg(spotCount)));
    spotList = new QListWidget();   //创建可滚动的列表控件
    leftLayout->addWidget(spotList);

    //绘制查询功能组框
    QGroupBox *queryGroup = new QGroupBox("查询功能");      //创建有边框的控件
    QVBoxLayout *queryLayout = new QVBoxLayout(queryGroup); //创建垂直布局管理器并绑定
    //创建三个按钮
    QPushButton *btnSpotInfo = new QPushButton("景点信息查询");
    QPushButton *btnShortestPath = new QPushButton("最短路径查询");
    QPushButton *btnAllPaths = new QPushButton("所有路径查询");
    //组装各类控件
    queryLayout->addWidget(btnSpotInfo);
    queryLayout->addWidget(btnShortestPath);
    queryLayout->addWidget(btnAllPaths);
    leftLayout->addWidget(queryGroup);

    //绘制按类型查询组框
    QGroupBox *typeGroup = new QGroupBox("按类型查询附近");      //创建有边框的控件
    QVBoxLayout *typeLayout = new QVBoxLayout(typeGroup);
    typeCombo = new QComboBox();    //创建下拉框
    // 加载10种类型到下拉框
    const char** typeNames = getTypeNames();
    //遍历类型数组，添加到下拉框中
    for (int i = 0; typeNames[i] != nullptr; i++) {
        typeCombo->addItem(typeNames[i]);
    }
    //创建按钮
    QPushButton *btnNearby = new QPushButton("查询附近景点");
    //组装各类控件
    typeLayout->addWidget(typeCombo);
    typeLayout->addWidget(btnNearby);
    leftLayout->addWidget(typeGroup);

    //绘制管理员功能组框（默认隐藏，管理员登录后显示）
    adminGroup = new QGroupBox("管理员功能"); //创建有边框的控件
    QVBoxLayout *adminLayout = new QVBoxLayout(adminGroup);
    //创建按钮
    QPushButton *btnAddSpot = new QPushButton("添加景点");
    btnUpdateSpot = new QPushButton("更新景点信息");
    QPushButton *btnDelSpot = new QPushButton("删除景点");
    QPushButton *btnModifyDist = new QPushButton("修改距离");
    QPushButton *btnRefresh = new QPushButton("刷新数据");
    //组装各类控件
    adminLayout->addWidget(btnAddSpot);
    adminLayout->addWidget(btnUpdateSpot);
    adminLayout->addWidget(btnDelSpot);
    adminLayout->addWidget(btnModifyDist);
    adminLayout->addWidget(btnRefresh);
    leftLayout->addWidget(adminGroup);
    adminGroup->hide();     //默认隐藏管理员功能区，普通用户不可见

    //管理员登录/退出按钮（始终可见，点击后弹出密码验证）
    btnAdminLogin = new QPushButton("管理员登录");
    btnAdminLogin->setStyleSheet("background-color: #ff9800; color: white; font-weight: bold; padding: 8px; border-radius: 6px;");
    leftLayout->addWidget(btnAdminLogin);

    //绘制退出按钮
    QPushButton *btnExit = new QPushButton("退出系统");
    leftLayout->addWidget(btnExit);
    leftLayout->addStretch();   //添加弹性空间，使底部留空更美观

    // 右侧地图控件
    mapWidget = new MapWidget();    //创建地图控件
    mainLayout->addWidget(leftPanel);
    mainLayout->addWidget(mapWidget, 1);

    //绑定信号槽，响应鼠标点击事件
    //连接方式：connect(信号发送者, 信号, 信号接收者, 槽函数)
    connect(btnSpotInfo, &QPushButton::clicked, this, &CampusNavWindow::onQuerySpotInfo);
    connect(btnShortestPath, &QPushButton::clicked, this, &CampusNavWindow::onQueryShortestPath);
    connect(btnAllPaths, &QPushButton::clicked, this, &CampusNavWindow::onQueryAllPaths);
    connect(btnNearby, &QPushButton::clicked, this, &CampusNavWindow::onQueryNearby);
    connect(btnAddSpot, &QPushButton::clicked, this, &CampusNavWindow::onAddSpot);
    connect(btnUpdateSpot, &QPushButton::clicked, this, &CampusNavWindow::onUpdateSpotAllInfo);
    connect(btnDelSpot, &QPushButton::clicked, this, &CampusNavWindow::onDeleteSpot);
    connect(btnModifyDist, &QPushButton::clicked, this, &CampusNavWindow::onModifyDistance);
    connect(btnRefresh, &QPushButton::clicked, this, &CampusNavWindow::onRefreshData);
    connect(btnAdminLogin, &QPushButton::clicked, this, &CampusNavWindow::onAdminLogin);
    connect(btnExit, &QPushButton::clicked, this, &QWidget::close);
    connect(spotList, &QListWidget::itemClicked, this, &CampusNavWindow::onSpotSelected);
}


//优化全局样式
void CampusNavWindow::applyStyleSheet() {
    this->setStyleSheet(R"(
        QMainWindow { background-color: #f5f5f5; }
        QWidget { background-color: #f5f5f5; color: #333333; font-family: "Microsoft YaHei"; }
        QLabel { color: #2c3e50; font-weight: bold; }
        QGroupBox {
            border: 1px solid #dcdcdc;
            border-radius: 8px;
            margin-top: 10px;
            padding-top: 10px;
            color: #2c3e50;
            font-weight: bold;
            background-color: #ffffff;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px 0 5px;
        }
        QPushButton {
            background-color: #e8e8e8;
            color: #333333;
            border: none;
            border-radius: 6px;
            padding: 8px;
            font-weight: bold;
        }
        QPushButton:hover { background-color: #4caf50; color: white; }
        QPushButton:pressed { background-color: #388e3c; }
        QListWidget {
            background-color: white;
            border: 1px solid #e0e0e0;
            border-radius: 6px;
            padding: 5px;
            color: #333333;
        }
        QListWidget::item { padding: 5px; border-radius: 4px; }
        QListWidget::item:selected { background-color: #4caf50; color: white; }
        QListWidget::item:hover { background-color: #f0f0f0; }
        QComboBox {
            background-color: white;
            border: 1px solid #e0e0e0;
            border-radius: 6px;
            padding: 6px;
            color: #333333;
        }
        QComboBox::drop-down { border: none; }
        QComboBox QAbstractItemView {
            background-color: white;
            color: #333333;
            selection-background-color: #4caf50;
            selection-color: white;
        }
    )");
}

//刷新景点列表
void CampusNavWindow::refreshSpotList() {
    spotList->clear();  //清空列表，清空再加载
    for (int i = 0; i < spotCount; i++) {
        spotList->addItem(QString("%1. %2").arg(spots[i].id).arg(QString::fromUtf8(spots[i].name)));
    }
}

//刷新地图
void CampusNavWindow::refreshMap() {
    mapWidget->updateMap(selectedStart, selectedEnd, pathNodes);
}

//获取类型字符串，转换为中文
QString CampusNavWindow::getTypeString(SpotType type) {
    char buf[128];
    typeToString(type, buf, sizeof(buf));
    return QString::fromUtf8(buf);
}

//查询景点信息
void CampusNavWindow::onQuerySpotInfo() {
    int idx = spotList->currentRow();   //返回当前选中的行索引
    if (idx < 0) {
        QMessageBox::warning(this, "提示", "请先在左侧列表中点击选择一个景点！");
        return;
    }
    QString info = QString(
        "========================================\n"
        "【%1】\n"
        "========================================\n"
        "编号：%2\n"
        "简介：%3\n"
        "坐标：( %4 , %5 )\n"
        "类型：%6\n" // 多类型分号分隔
        "========================================")
        .arg(QString::fromUtf8(spots[idx].name))
        .arg(spots[idx].id)
        .arg(QString::fromUtf8(spots[idx].desc))
        .arg(spots[idx].x)
        .arg(spots[idx].y)
        .arg(getTypeString(spots[idx].type));
    //显示对话框信息
    QMessageBox::information(this, "景点信息", info);
}

//查询最短距离
void CampusNavWindow::onQueryShortestPath() {
    if (spotCount < 2) {
        QMessageBox::warning(this, "提示", "景点数量不足");
        return;
    }

    //创建字符串列表
    QStringList names;
    for (int i = 0; i < spotCount; i++) {
        names << QString::fromUtf8(spots[i].name);  //追加元素到列表
    }
    bool ok;    //声明确认标志，用于判断用户是否确认
    QString startName = QInputDialog::getItem(this, "选择起点", "起点:", names, 0, false, &ok);
    if (!ok) return;    //用户取消选择，则关闭对话框退出函数
    QString endName = QInputDialog::getItem(this, "选择终点", "终点:", names, 0, false, &ok);
    if (!ok) return;
    //查找景点下标
    int start = findSpotByName(startName.toUtf8().data());
    int end = findSpotByName(endName.toUtf8().data());
    //验证景点是否存在
    if (start < 0 || end < 0) {
        QMessageBox::warning(this, "错误", "找不到景点");
        return;
    }
    //保存起点、终点
    selectedStart = start;
    selectedEnd = end;
    //声明距离和前驱数组
    int dist[MAX], prev[MAX];
    dijkstra(start, end, dist, prev);
    //无法到达情况
    if (dist[end] == INF) {
        QMessageBox::information(this, "结果", "无法到达");
        pathNodes.clear();  // 清空路径列表，地图上不高亮任何路径
    }
    else //有路径
        {
        pathNodes.clear();
        int cur = end;
        //回溯路径
        while (cur != -1) {
            pathNodes.push_front(cur);  //添加节点到路径列表前面
            cur = prev[cur];
        }
        QString pathStr;
        for (int i = 0; i < pathNodes.size(); i++) {
            if (i > 0) pathStr += " → ";
            pathStr += QString::fromUtf8(spots[pathNodes[i]].name);
        }
        QMessageBox::information(this, "最短路径",
            QString("路径: %1\n总距离: %2 米").arg(pathStr).arg(dist[end]));
    }
    refreshMap();//触发地图重绘，高亮显示
}

//查询所有路径
void CampusNavWindow::onQueryAllPaths() {
    if (spotCount < 2) {
        QMessageBox::warning(this, "提示", "景点数量不足");
        return;
    }
    QStringList names;
    for (int i = 0; i < spotCount; i++) {
        names << QString::fromUtf8(spots[i].name);
    }
    bool ok;
    QString startName = QInputDialog::getItem(this, "选择起点", "起点:", names, 0, false, &ok);
    if (!ok) return;
    QString endName = QInputDialog::getItem(this, "选择终点", "终点:", names, 0, false, &ok);
    if (!ok) return;
    int start = findSpotByName(startName.toUtf8().data());
    int end = findSpotByName(endName.toUtf8().data());
    if (start < 0 || end < 0) {
        QMessageBox::warning(this, "错误", "找不到景点");
        return;
    }

    //DFS收集所有路径
    QStringList allPaths;
    int visited[MAX] = {0}; //标记节点是否已访问
    int path[MAX];      //记录当前路径上的节点序列
    std::function<void(int, int)> dfs = [&](int cur, int depth) {
       //终止条件，到达终点
        if (cur == end) {
            QString pathStr;    //创建数组，拼接景点名称
            int totalDistance = 0;
            for (int i = 0; i <= depth; i++) {
                if (i > 0) {
                    pathStr += " → ";
                    totalDistance += graph[path[i-1]][path[i]]; //累加距离
                }
                pathStr += QString::fromUtf8(spots[path[i]].name);
            }
            pathStr += QString(" (距离: %1 米)").arg(totalDistance);
            allPaths.append(pathStr);//把路径添加到列表中
            return;
        }
        visited[cur] = 1;   //标记当前节点已访问
        for (int i = 0; i < spotCount; i++) {
            //遍历所有景点，存在边且未访问过
            if (graph[cur][i] != INF && graph[cur][i] > 0 && !visited[i]) {
                path[depth + 1] = i;
                //递归调用dfs
                dfs(i, depth + 1);
            }

        }
        visited[cur] = 0;   //回溯作用：恢复当前节点为未访问状态，以便其他路径可以经过
    };

    path[0] = start;
    dfs(start, 0);

    //构建结果字符串
    QString result = QString("从【%1】到【%2】共找到 %3 条路径：\n\n").arg(startName).arg(endName).arg(allPaths.size());
    for (int i = 0; i < allPaths.size(); i++) {
        //遍历所有路径，添加编号和路径详情
        result += QString("%1. %2\n").arg(i + 1).arg(allPaths[i]);
    }

    //绘制对话框完整显示所有路径
    QDialog *dialog = new QDialog(this);    //创建对话框
    dialog->setWindowTitle("所有路径");
    dialog->resize(700, 500);
    dialog->setAttribute(Qt::WA_DeleteOnClose);  //关闭后自动释放内存

    QVBoxLayout *layout = new QVBoxLayout(dialog);  //创建垂直布局
    QTextEdit *textEdit = new QTextEdit(dialog);    //创建能滚动的QTextEdit控件显示所有路径
    textEdit->setReadOnly(true);    //只读模式
    textEdit->setPlainText(result); //添加文本内容
    layout->addWidget(textEdit);

    QPushButton *closeBtn = new QPushButton("关闭", dialog);
    layout->addWidget(closeBtn, 0, Qt::AlignCenter);

    connect(closeBtn, &QPushButton::clicked, dialog, &QDialog::accept);
    dialog->exec(); //以模态方式显示对话框（阻塞直到用户关闭）
}

//查询附近景点
void CampusNavWindow::onQueryNearby() {
    //1.获取目标类型信息
    int targetTypeIdx = typeCombo->currentIndex();  //获取当前选中的下拉框索引
    const char** typeNames = getTypeNames();        //获取所有类型名称数组
    QString typeName = typeCombo->currentText();    //获取当前类型名称
    // 映射下拉框下标 → 位标记
    SpotType targetType = (SpotType)(1 << targetTypeIdx);

    //2.选择当前位置
    QStringList names;
    for (int i = 0; i < spotCount; i++) {
        names << QString::fromUtf8(spots[i].name);
    }
    bool ok;    //用于判断是否点击了对话框的"确定"按钮

    //Qt的静态方法，显示一个下拉选择对话框，输入分别为（父窗口，标题，对话框内提示文字，选项列表，默认选项索引，不允许编辑，点"确定"则为true，否则反之）
    QString currentName = QInputDialog::getItem(this, "选择当前位置", "当前位置:", names, 0, false, &ok);
    if (!ok) return;    //点击取消则返回
    //将用户选择的名称转换为数组下标
    int current = findSpotByName(currentName.toUtf8().data());
    if (current < 0) {
        QMessageBox::warning(this, "错误", "找不到景点");
        return;
    }
    //创建一个列表，存储所有符合条件的景点及其距离
    QList<QPair<int, int>> targetDistances;
    //找出所有对应景点的最短距离
    for (int i = 0; i < spotCount; i++) {
        // 首先判断景点是否包含目标类型，采用位运算
        if ((spots[i].type & targetType) && i != current) {
            int dist[MAX], prev[MAX];
            //用dijkstra算法计算最短距离
            dijkstra(current, i, dist, prev);
            if (dist[i] != INF) {
                //qMakePair(i, dist[i])：创建QPair对象，存储景点索引和距离
                targetDistances.append(qMakePair(i, dist[i]));
            }
        }
    }

    //3.处理无结果的情况
    if (targetDistances.isEmpty()) {
        QMessageBox::information(this, "结果", QString("附近没有找到%1").arg(typeName));
        return;
    }

   //4.将找到的最短路径进行冒泡排序
    for (int i = 0; i < targetDistances.size() - 1; i++) {
        for (int j = 0; j < targetDistances.size() - 1 - i; j++) {
            if (targetDistances[j].second > targetDistances[j + 1].second) {
                // 交换两个元素
                QPair<int, int> temp = targetDistances[j];
                targetDistances[j] = targetDistances[j + 1];
                targetDistances[j + 1] = temp;
            }
        }
    }


    //5.构建结果字符串
    QString result = QString("从【%1】出发到附近%2：\n\n").arg(currentName).arg(typeName);

    for (int i = 0; i < targetDistances.size(); i++) {
        int target = targetDistances[i].first;
        int distance = targetDistances[i].second;
        result += QString("%1. %2 : %3 米\n").arg(i + 1).arg(QString::fromUtf8(spots[target].name)).arg(distance);
    }
    int nearestTarget = targetDistances.first().first;
    int nearestDist = targetDistances.first().second;

    //6.提取最近景点并询问用户
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "附近景点查询",
        result + QString("\n\n最近的%1是【%2】，距离 %3 米。\n是否在地图上显示路径？")
            .arg(typeName, QString::fromUtf8(spots[nearestTarget].name)).arg(nearestDist),
        QMessageBox::Yes | QMessageBox::No
    );
    if (reply == QMessageBox::Yes) {
        selectedStart = current;
        selectedEnd = nearestTarget;
        int dist[MAX], prev[MAX];
        dijkstra(current, nearestTarget, dist, prev);
        if (dist[nearestTarget] != INF) {
            pathNodes.clear();
            int cur = nearestTarget;
            //当回溯到起点时停止
            while (cur != -1) {
                pathNodes.push_front(cur);
                cur = prev[cur];
            }
            refreshMap();
            QString pathStr;
            for (int i = 0; i < pathNodes.size(); i++) {
                if (i > 0) pathStr += " → ";
                pathStr += QString::fromUtf8(spots[pathNodes[i]].name);
            }
            QMessageBox::information(this, "路径详情",
                QString("到【%1】的最短路径：\n%2\n总距离：%3 米")
                .arg(QString::fromUtf8(spots[nearestTarget].name))
                .arg(pathStr).arg(dist[nearestTarget]));
        }
    }
}

// ===================== 管理员功能 - 添加景点（多选类型核心） =====================
void CampusNavWindow::onAddSpot() {
    bool ok;
    // 1. 录入基础信息
    //输入景点名称
    QString name = QInputDialog::getText(this, "添加景点", "景点名称:", QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty()) return;
    //输入景点简介
    QString desc = QInputDialog::getText(this, "添加景点", "景点简介:", QLineEdit::Normal, "", &ok);
    if (!ok) return;

    // 2. ========== 多选类型（复选框组）==========
    QDialog typeDialog(this);
    typeDialog.setWindowTitle("选择景点类型（可多选）");
    QVBoxLayout *typeLayout = new QVBoxLayout(&typeDialog);
    QList<QCheckBox*> checkBoxList;
    const char** typeNames = getTypeNames();

    // 动态创建10个复选框
    for (int i = 0; typeNames[i] != nullptr; i++) {
        QCheckBox *cb = new QCheckBox(typeNames[i]);
        checkBoxList.append(cb);
        typeLayout->addWidget(cb);
    }
    QPushButton *okBtn = new QPushButton("确定");
    typeLayout->addWidget(okBtn);
    connect(okBtn, &QPushButton::clicked, &typeDialog, &QDialog::accept);
    if (typeDialog.exec() != QDialog::Accepted) return;

    // 收集选中项 → 转为位标记
    SpotType newType = TYPE_NONE;
    for (int i = 0; i < checkBoxList.size(); i++) {
        if (checkBoxList[i]->isChecked()) {
            newType = (SpotType)( (int)newType | (1 << i) );
        }
    }

    // 3. 录入坐标
    //输入x坐标
    int x = QInputDialog::getInt(this, "添加景点", "X坐标:", 100, 0, 1000, 1, &ok);
    if (!ok) return;
    //输入y坐标
    int y = QInputDialog::getInt(this, "添加景点", "Y坐标:", 100, 0, 800, 1, &ok);
    if (!ok) return;

    // 4. 距离数组初始化
    int distances[MAX] = {0};       //连通距离初始化全化为0
    //无景点直接新增
    if (spotCount == 0) {
        Spot newSpot;
        memset(newSpot.name, 0, sizeof(newSpot.name));
        memset(newSpot.desc, 0, sizeof(newSpot.desc));
        QByteArray nameBytes = name.toUtf8();
        QByteArray descBytes = desc.toUtf8();
        strncpy(newSpot.name, nameBytes.data(), sizeof(newSpot.name) - 1);
        strncpy(newSpot.desc, descBytes.data(), sizeof(newSpot.desc) - 1);
        newSpot.type = newType; // 多选类型位标记
        newSpot.x = x;
        newSpot.y = y;
        addSpot(newSpot, distances);
        refreshSpotList();
        refreshMap();
        QMessageBox::information(this, "成功", "景点添加成功");
        return;
    }

    // 5. 循环选择连通的旧景点并录入距离
    while (true) {
        QStringList names;
        for (int i = 0; i < spotCount; i++) {
            names << QString("%1. %2").arg(i+1).arg(QString::fromUtf8(spots[i].name));
        }
        names << "退出选择";
        QString selectName = QInputDialog::getItem(
            this, "选择相连景点",
            "请选择和新景点【直接连通】的景点\n选「Cancel」结束录入",
            names, 0, false, &ok);
        if (!ok || selectName == "退出选择") break;
        int idx = selectName.split(".")[0].toInt() - 1;
        int dist = QInputDialog::getInt(
            this, "输入道路距离",
            QString("到【%1】的距离(米)").arg(QString::fromUtf8(spots[idx].name)),
            100, 1, 9999, 1, &ok);
        if (!ok) break;
        distances[idx] = dist;
    }

    // 6. 组装景点结构体并添加
    Spot newSpot;
    memset(newSpot.name, 0, sizeof(newSpot.name));
    memset(newSpot.desc, 0, sizeof(newSpot.desc));
    QByteArray nameBytes = name.toUtf8();
    QByteArray descBytes = desc.toUtf8();
    strncpy(newSpot.name, nameBytes.data(), sizeof(newSpot.name) - 1);
    strncpy(newSpot.desc, descBytes.data(), sizeof(newSpot.desc) - 1);
    newSpot.type = newType;
    newSpot.x = x;
    newSpot.y = y;
    addSpot(newSpot, distances);

    //刷新界面
    refreshSpotList();
    refreshMap();
    QMessageBox::information(this, "成功", "景点添加完成");
}

//调用delSpot删除景点
void CampusNavWindow::onDeleteSpot() {
    if (spotCount == 0) {
        QMessageBox::warning(this, "提示", "没有景点可删除");
        return;
    }
    QStringList names;
    for (int i = 0; i < spotCount; i++) {
        names << QString::fromUtf8(spots[i].name);
    }
    bool ok;
    QString name = QInputDialog::getItem(this, "删除景点", "选择要删除的景点:", names, 0, false, &ok);
    if (!ok) return;
    int idx = findSpotByName(name.toUtf8().data());
    if (idx < 0) return;
    delSpot(idx);           //调用管理员删除窗口

    //清空路径标亮标记
    selectedStart = selectedEnd = -1;
    pathNodes.clear();
    refreshSpotList();
    refreshMap();
    QMessageBox::information(this, "成功", "景点删除成功");
}

/**
 * @brief 修改两点间路径（增/删/改）
 */
void CampusNavWindow::onModifyDistance() {
    bool ok;
    QStringList spotNames;
    for (int i = 0; i < spotCount; i++) {
        spotNames << QString("%1. %2").arg(i+1).arg(QString::fromUtf8(spots[i].name));
    }
    if (spotNames.size() < 2) {
        QMessageBox::warning(this, "提示", "景点数量不足，无法修改路径！");
        return;
    }
    QString startStr = QInputDialog::getItem(this, "修改路径", "选择起点:", spotNames, 0, false, &ok);
    if (!ok) return;
    int startIdx = startStr.split(".")[0].toInt() - 1;
    QString endStr = QInputDialog::getItem(this, "修改路径", "选择终点:", spotNames, 0, false, &ok);
    if (!ok || startIdx == spotNames.indexOf(endStr)) return;
    int endIdx = endStr.split(".")[0].toInt() - 1;

    QStringList ops = {"新增/修改路径（设置距离）", "删除路径（设为不可达）"};
    QString op = QInputDialog::getItem(this, "操作选择", "请选择操作:", ops, 0, false, &ok);
    if (!ok) return;
    int newDist = INF;
    if (op == ops[0]) {
        newDist = QInputDialog::getInt(this, "设置距离", "输入两点间距离(米):", 100, 1, 9999, 1, &ok);
        if (!ok) return;
    }

    //调用管理员修改道路接口
    modifyDistance(startIdx, endIdx, newDist);
    refreshMap();
    QMessageBox::information(this, "成功", "路径修改完成！");
}

// ===================== 管理员功能 - 更新景点（多选类型核心） =====================
/**
 * @brief 更新景点信息并选择是否清空原有路径
 * 流程：选景点 → 改名称/简介/类型/坐标 → 选择是否清空旧路径
 *  1. 不清空：直接保存退出
 *  2. 清空：清空旧路径 → 手动添加新路径 → 保存
 */
void CampusNavWindow::onUpdateSpotAllInfo()
{
    bool ok;
    // 1. 选择要修改的景点
    QStringList spotListStr;
    for (int i = 0; i < spotCount; ++i)
    {
        spotListStr << QString("%1. %2").arg(i+1).arg(QString::fromUtf8(spots[i].name));
    }
    if (spotListStr.isEmpty())
    {
        QMessageBox::warning(this, "提示", "暂无景点数据！");
        return;
    }
    QString selectStr = QInputDialog::getItem(this, "选择景点", "请选择要修改的景点：", spotListStr, 0, false, &ok);
    if (!ok) return;
    int spotId = selectStr.split(".").at(0).toInt() - 1;

    // 2. 修改景点名称
    QString newName = QInputDialog::getText(this, "修改名称", "请输入新景点名称：",
                                            QLineEdit::Normal, QString::fromUtf8(spots[spotId].name), &ok);
    if (!ok) return;

    // 3. 修改景点简介
    QString newDesc = QInputDialog::getText(this, "修改简介", "请输入新景点简介：",
                                            QLineEdit::Normal, QString::fromUtf8(spots[spotId].desc), &ok);
    if (!ok) return;

    // 4. 重新选择景点类型（支持多选，回显原有选项）
    QDialog typeDialog(this);
    typeDialog.setWindowTitle("重新选择景点类型（可多选）");
    QVBoxLayout *typeLayout = new QVBoxLayout(&typeDialog);
    QList<QCheckBox*> checkBoxList;
    const char** typeNames = getTypeNames();

    for (int i = 0; typeNames[i] != nullptr; i++)
    {
        QCheckBox *cb = new QCheckBox(typeNames[i]);
        // 根据原有位标记勾选对应框
        if (spots[spotId].type & (SpotType)(1 << i))
        {
            cb->setChecked(true);
        }
        checkBoxList.append(cb);
        typeLayout->addWidget(cb);
    }
    QPushButton *okBtn = new QPushButton("确定");
    typeLayout->addWidget(okBtn);
    connect(okBtn, &QPushButton::clicked, &typeDialog, &QDialog::accept);
    if (typeDialog.exec() != QDialog::Accepted) return;

    // 收集新的类型位标记
    SpotType newType = TYPE_NONE;
    for (int i = 0; i < checkBoxList.size(); i++)
    {
        if (checkBoxList[i]->isChecked())
        {
            // 兼容枚举位运算（两种写法任选其一，已做类型强转）
            newType = static_cast<SpotType>(static_cast<int>(newType) | (1 << i));
        }
    }

    // 5. 修改 X / Y 坐标
    int newX = QInputDialog::getInt(this, "修改坐标", "输入新 X 坐标：",
                                    spots[spotId].x, 0, 2000, 1, &ok);
    if (!ok) return;
    int newY = QInputDialog::getInt(this, "修改坐标", "输入新 Y 坐标：",
                                    spots[spotId].y, 0, 2000, 1, &ok);
    if (!ok) return;

    // 6. 先更新景点基础信息（名称、简介、类型）
    char nameBuf[20] = {0};
    char descBuf[100] = {0};
    strncpy(nameBuf, newName.toUtf8().data(), sizeof(nameBuf)-1);
    strncpy(descBuf, newDesc.toUtf8().data(), sizeof(descBuf)-1);
    updateSpotInfo(spotId, nameBuf, descBuf, newType);

    // 7. 更新坐标并保存基础数据
    spots[spotId].x = newX;
    spots[spotId].y = newY;
    saveAll();

    // 8.弹窗选择是否清空原有路径
    QMessageBox::StandardButton btn = QMessageBox::question(
        this,
        "路径处理选择",
        "是否清空该景点原有的所有连通路径？\n"
        "【Yes】清空旧路径，接下来重新添加新路径\n"
        "【No】保留原有所有路径，直接完成更新",
        QMessageBox::Yes | QMessageBox::No
    );

    // 选择【No】：不清空路径，直接结束流程
    if (btn == QMessageBox::No)
    {
        refreshSpotList();
        refreshMap();
        QMessageBox::information(this, "操作完成", "景点信息已更新，原有路径保留！");
        return;
    }

    // 选择【Yes】：执行清空旧路径 + 手动添加新路径
    QMessageBox::information(this, "操作提示", "已清空原有路径，请重新添加连通路径！");
    clearAllEdgeOfSpot(spotId);     //调用清空路径函数
    // 循环录入新景点连通距离
    int distArr[MAX] = {0};
    while (true)
    {
        QStringList otherSpotList;
        for (int i = 0; i < spotCount; ++i)
        {
            if (i == spotId) continue;
            otherSpotList << QString("%1. %2").arg(i+1).arg(QString::fromUtf8(spots[i].name));
        }
        otherSpotList << "完成添加";
        QString choose = QInputDialog::getItem(this, "添加新路径",
                                               "选择要连通的景点（选【Cancel】结束）：",
                                               otherSpotList, 0, false, &ok);
        if (!ok || choose == "完成添加") break;

        int targetIdx = choose.split(".").at(0).toInt() - 1;
        int dist = QInputDialog::getInt(this, "设置距离", "输入两点之间距离(米)：",
                                        100, 1, 9999, 1, &ok);
        if (!ok) break;

        distArr[targetIdx] = dist;
    }

    // 9. 批量写入新路径到邻接矩阵
    for (int i = 0; i < spotCount; ++i)
    {
        if (distArr[i] > 0)
        {
            modifyDistance(spotId, i, distArr[i]);
        }
    }

    // 10. 刷新界面
    refreshSpotList();
    refreshMap();
    QMessageBox::information(this, "操作完成", "景点信息与新路径更新完毕！");
}

/**
 * @brief 刷新文件数据（重新加载）
 */
void CampusNavWindow::onRefreshData() {
    loadSpots("data/spots.txt");
    loadDistance("data/distance.txt");
    selectedStart = selectedEnd = -1;
    pathNodes.clear();
    refreshSpotList();
    refreshMap();
    QMessageBox::information(this, "成功", QString("数据已刷新，共 %1 个景点").arg(spotCount));
}

/**
 * @brief 点击列表景点，设为路径起点
 */
void CampusNavWindow::onSpotSelected(QListWidgetItem *item) {
    selectedStart = spotList->row(item);
    selectedEnd = -1;
    refreshMap();
}

/**
 * @brief 管理员登录/退出
 * 未登录时：弹出密码输入框，验证通过后显示管理员功能区
 * 已登录时：隐藏管理员功能区，恢复普通用户界面
 * 默认管理员密码：admin123
 */
void CampusNavWindow::onAdminLogin() {
    if (isAdmin) {
        //已登录，退出管理员模式
        adminGroup->hide();
        isAdmin = false;
        btnAdminLogin->setText("管理员登录");
        btnAdminLogin->setStyleSheet("background-color: #ff9800; color: white; font-weight: bold; padding: 8px; border-radius: 6px;");
        QMessageBox::information(this, "提示", "已退出管理员模式");
    } else {
        //未登录，弹出密码输入对话框
        bool ok;
        QString password = QInputDialog::getText(
            this, "管理员登录", "请输入管理员密码:", QLineEdit::Password, "", &ok);
        if (!ok || password.isEmpty()) return;

        //验证密码（默认密码: admin123）
        if (password == "admin123") {
            adminGroup->show();
            isAdmin = true;
            btnAdminLogin->setText("退出管理员");
            btnAdminLogin->setStyleSheet("background-color: #f44336; color: white; font-weight: bold; padding: 8px; border-radius: 6px;");
            QMessageBox::information(this, "成功", "管理员登录成功！");
        } else {
            QMessageBox::warning(this, "错误", "密码错误！");
        }
    }
}
