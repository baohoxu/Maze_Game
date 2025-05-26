#include "maze.h"

extern int win;
extern double winWidth, winHeight;
extern int playerX, playerY;
extern int targetX, targetY;
extern int maze[MAZESIZE][MAZESIZE];
extern point mazePoint[MAZESIZE][MAZESIZE];
extern double mouseX, mouseY;
extern queue* path;
extern int isAnswering;
extern int isNexting;
extern int isEditing;
extern FileNode* fileName;

int hasBegin = 0;
int health = 100;
int hasDoor = 0;
int hasKey = 0;
int leftKey = 0;
int fakeLeftKey = 0;
int element = 1;
int isChoosingElement = 0;
int isOpening = 0;
int isRenaming = 0;
int isPlaying = 0;
int isHelping = 0;
int isAbouting = 0;
int isShowing = 0;
static char name[21] = "未命名";

queue* show;
int showVisited[MAZESIZE][MAZESIZE];
int fakeMaze[MAZESIZE][MAZESIZE];

//计时器
void TimerEventProcess(int TimerID) {

    if(TimerID == HEALTH && hasBegin) {
        --health;
        if(!health) win = -1;
    }

    if(TimerID == FLUSH) display();

    if(TimerID == PLAY && isPlaying) {

        hasAns();
        path = reverse(path);

        playerX = path->x;
        playerY = path->y;

        if(maze[playerX][playerY] == 4 && leftKey) {
            --leftKey;
            maze[playerX][playerY] = 0;
        }else if(maze[playerX][playerY] == 5) {
            ++leftKey;
            maze[playerX][playerY] = 0;
        }

        if(playerX == targetX && playerY == targetY) isPlaying = 0;

    }

    if(TimerID == ANSWER && isShowing) {

        int k = count(show);
        if(!k) return;

        int x, y;

        for(int i = 0; i < k; i++) {
            x = show->next->x;
            y = show->next->y;

            if(fakeMaze[x][y] == 4 && fakeLeftKey || fakeMaze[x][y] == 5) {

                memset(showVisited, 0, sizeof(showVisited));
                if(fakeMaze[x][y] == 5) {
                    ++fakeLeftKey;
                    fakeMaze[x][y] = 0;
                }else{
                    --fakeLeftKey;
                    fakeMaze[x][y] = 0;
                }

            }

            if(x == targetX && y == targetY) isShowing = 0;

            if((!fakeMaze[x+1][y] || fakeMaze[x+1][y] == 5 || fakeMaze[x+1][y] == 4 && fakeLeftKey) && !showVisited[x+1][y]) {
                push(show, show->next, x+1, y);
                showVisited[x+1][y] = 1;
            }

            if((!fakeMaze[x][y+1] || fakeMaze[x][y+1] == 5 || fakeMaze[x][y+1] == 4 && fakeLeftKey) && !showVisited[x][y+1]) {
                push(show, show->next, x, y+1);
                showVisited[x][y+1] = 1;
            }

            if((!fakeMaze[x-1][y] || fakeMaze[x-1][y] == 5 || fakeMaze[x-1][y] == 4 && fakeLeftKey) && !showVisited[x-1][y]) {
                push(show, show->next, x-1, y);
                showVisited[x-1][y] = 1;
            }

            if((!fakeMaze[x][y-1] || fakeMaze[x][y-1] == 5 || fakeMaze[x][y-1] == 4 && fakeLeftKey) && !showVisited[x][y-1]) {
                push(show, show->next, x, y-1);
                showVisited[x][y-1] = 1;
            }

            pop(show);

        }

    }

    if(isShowing) {

        int k = count(show);
        if(!k) return;

        queue* p = show;

        int x, y;

        SetPenColor("Green");

        for(int i = 0; i < k; i++) {

            x = p->next->x;
            y = p->next->y;

            drawPoint(mazePoint[x][y].x, mazePoint[x][y].y);

            p = p->next;

        }
    }
}

//字符控制
void CharEventProcess(char ch) {
    uiGetChar(ch);
    display();
}

//键盘控制
void KeyboardEventProcess(int key, int event) {
    uiGetKeyboard(key,event);

    if(event == BUTTON_DOWN && key == VK_ESCAPE) {

        isChoosingElement = 0;
        isOpening = 0;
        isPlaying = 0;
        isHelping = 0;
        isAbouting = 0;
        isAnswering = 0;
        isNexting = 0;
        isEditing = 0;
        isShowing = 0;

    }

    if(event == BUTTON_DOWN && key == VK_RETURN) isRenaming = !isRenaming;

    if(event == BUTTON_DOWN && !win && !isRenaming) {
        if(!isEditing) {

            if (key == 'W' || key == VK_UP) {
                if (!maze[playerX][playerY+1]) playerY += 1;
                else if (maze[playerX][playerY+1] == 4) {
                    if(leftKey) {
                        --leftKey;
                        playerY += 1;
                        maze[playerX][playerY] = 0;
                    }
                }else if (maze[playerX][playerY+1] == 5) {
                    ++leftKey;
                    playerY += 1;
                    maze[playerX][playerY] = 0;
                }
                isNexting = 0;
                hasBegin = 1;
            }

            if (key == 'D' || key == VK_RIGHT) {
                if (!maze[playerX+1][playerY]) playerX += 1;
                else if (maze[playerX+1][playerY] == 4) {
                    if(leftKey) {
                        --leftKey;
                        playerX += 1;
                        maze[playerX][playerY] = 0;
                    }
                }else if (maze[playerX+1][playerY] == 5) {
                    ++leftKey;
                    playerX += 1;
                    maze[playerX][playerY] = 0;
                }
                isNexting = 0;
                hasBegin = 1;
            }

            if (key == 'S' || key == VK_DOWN) {
                if (!maze[playerX][playerY-1]) playerY -= 1;
                else if (maze[playerX][playerY-1] == 4) {
                    if(leftKey) {
                        --leftKey;
                        playerY -= 1;
                        maze[playerX][playerY] = 0;
                    }
                }else if (maze[playerX][playerY-1] == 5) {
                    ++leftKey;
                    playerY -= 1;
                    maze[playerX][playerY] = 0;
                }
                isNexting = 0;
                hasBegin = 1;
            }

            if (key == 'A' || key == VK_LEFT) {
                if (!maze[playerX-1][playerY]) playerX -= 1;
                else if (maze[playerX-1][playerY] == 4) {
                    if(leftKey) {
                        --leftKey;
                        playerX -= 1;
                        maze[playerX][playerY] = 0;
                    }
                }else if (maze[playerX-1][playerY] == 5) {
                    ++leftKey;
                    playerX -= 1;
                    maze[playerX][playerY] = 0;
                }
                isNexting = 0;
                hasBegin = 1;
            }
        }
    }

}

//鼠标控制
void MouseEventProcess(int x, int y, int button, int event) {
    uiGetMouse(x, y, button, event);

    mouseX = ScaleXInches(x);
    mouseY = ScaleYInches(y);

    if(button == LEFT_BUTTON && event == BUTTON_DOWN) {
        if (isEditing) {
            for (int i = 1; i < MAZESIZE - 1; i++) {
                for (int j = 1; j < MAZESIZE - 1; j++) {
                    if (fabs(mouseX - mazePoint[i][j].x) <= GRIDSIZE / 2 && fabs(mouseY - mazePoint[i][j].y) <= GRIDSIZE / 2) {
                        if(element == 2 && !maze[i][j] && !(i == targetX && j == targetY)) {
                            playerX = i;
                            playerY = j;
                        }else if(element == 3 && !maze[i][j] && !(i == playerX && j == playerY)) {
                            targetX = i;
                            targetY = j;
                        }else if(!(i == playerX && j == playerY) && !(i == targetX && j == targetY))
                            maze[i][j] = element;

                        i = MAZESIZE;
                        break;
                    }
                }
            }
        }
    }

}

//把数字转为字符串
string IntToString(int n) {

    if(!n) return "0";

    int k = 1;
    while(pow(10, k) <= n) ++k;

    string ret = (string) malloc((k + 1) * sizeof(char));
    ret[k] = '\0';

    while(n) {

        ret[--k] = n % 10 + '0';
        n /= 10;

    }

    return ret;
}

//广度优先搜索
void bfs(queue* p, int** visited, int steps, int* ret, int target) {

    int k = count(p);
    if(!k) return;

    int x, y;


    for(int i = 0; i < k; i++) {
        x = p->next->x;
        y = p->next->y;

        if(target == 4 && maze[x][y] == 4 && leftKey || target == 5 && maze[x][y] == 5 || !target && x == targetX && y == targetY) {
            copy(path, p->next);
            *ret = steps;
            return;
        }

        if((!maze[x+1][y] || maze[x+1][y] == 5 || maze[x+1][y] == 4 && leftKey) && !visited[x+1][y]) {
            push(p, p->next, x+1, y);
            visited[x+1][y] = 1;
        }

        if((!maze[x][y+1] || maze[x][y+1] == 5 || maze[x][y+1] == 4 && leftKey) && !visited[x][y+1]) {
            push(p, p->next, x, y+1);
            visited[x][y+1] = 1;
        }

        if((!maze[x-1][y] || maze[x-1][y] == 5 || maze[x-1][y] == 4 && leftKey) && !visited[x-1][y]) {
            push(p, p->next, x-1, y);
            visited[x-1][y] = 1;
        }

        if((!maze[x][y-1] || maze[x][y-1] == 5 || maze[x][y-1] == 4 && leftKey) && !visited[x][y-1]) {
            push(p, p->next, x, y-1);
            visited[x][y-1] = 1;
        }

        pop(p);

    }

    bfs(p, visited, steps+1, ret, target);

}

//解迷宫
int hasAns() {

    queue* currPath = (queue*) malloc(sizeof(queue));
    currPath->next = NULL;

    int p[MAZESIZE][MAZESIZE];
    memset(p, 0, sizeof(p));

    int** visited= (int**) malloc(MAZESIZE * sizeof(int*));
    for(int i = 0; i < MAZESIZE; i++) visited[i] = p[i];

    push(currPath, NULL, playerX, playerY);
    visited[playerX][playerY] = 1;
    int ret = 0;
    bfs(currPath, visited, 0, &ret, 0);

    queue *way = path->next;
    int doors = 0;

    if(ret) {
        while (way->next) {
            way = way->next;
            int px = way->x, py = way->y;
            if (maze[px][py] == 4) ++doors;
        }
    }

    if(doors > leftKey) ret = 0;

    if(!ret) {
        memset(p, 0, sizeof(p));
        currPath->next = NULL;
        push(currPath, NULL, playerX, playerY);
        bfs(currPath, visited, 0, &ret, 5);
    }

    if(!ret) {
        memset(p, 0, sizeof(p));
        currPath->next = NULL;
        push(currPath, NULL, playerX, playerY);
        bfs(currPath, visited, 0, &ret, 4);
    }

    return ret;

}

//随机生成迷宫
void initMaze() {

    for(int i = 0; i < MAZESIZE; i++) {
        maze[i][0] = 1;
        maze[0][i] = 1;
        maze[MAZESIZE-1][i] = 1;
        maze[i][MAZESIZE-1] = 1;
    }

    for(int i = 1; i < MAZESIZE - 1; i++) {
        for(int j = 1; j < MAZESIZE - 1; j++) {
            maze[i][j] = rand() % 2;
        }
    }

    while(maze[playerX][playerY] == 1 || maze[targetX][targetY] == 1) {
        playerX = rand() % (MAZESIZE - 1) + 1;
        playerY = rand() % (MAZESIZE - 1) + 1;
        targetX = rand() % (MAZESIZE - 1) + 1;
        targetY = rand() % (MAZESIZE - 1) + 1;
    }

}

//记录地图每个格子的位置
void initMazePoint() {
    for(int i = 0; i < MAZESIZE; i++) {
        for(int j = 0; j < MAZESIZE; j++) {
            mazePoint[i][j].x = 2 + (GRIDSIZE + 0.02) * i + GRIDSIZE / 2;
            mazePoint[i][j].y = 0.3 + (GRIDSIZE + 0.02) * j + GRIDSIZE / 2;
        }
    }

    initMaze();
    while(hasAns() <= 30) initMaze();
}

//设置一个门
void setADoor() {

    queue* p = path;
    int px, py;
    int n = 5;

    while(n--) p = p->next;

    while(p->next) {
        p = p->next;
        px = p->x;
        py = p->y;
        if(maze[px+1][py] == 1 && maze[px-1][py] == 1 || maze[px][py+1] == 1 && maze[px][py-1] == 1) {
            maze[px][py] = 4;
            break;
        }
    }

    hasDoor = 1;
}

//设置一个钥匙
void setAKey() {

    int kx = rand() % (MAZESIZE - 2) + 1;
    int ky = rand() % (MAZESIZE - 2) + 1;

    if(!maze[kx][ky]) {
        maze[kx][ky] = 5;
        if(hasAns() < 10) {
            maze[kx][ky] = 0;
            setAKey();
        }
    }else setAKey();

    hasKey = 1;
}

//显示剩余钥匙数
void drawLeftKeys() {

    SetPenColor("Black");
    SetPointSize(20);

    MovePen(0.7, winHeight - 0.7 - GetFontHeight() * 12);
    DrawTextString("剩余钥匙:");

    DrawTextString(IntToString(leftKey));

}

//绘制剩余血量
void drawHealth() {

    SetPenColor("Black");

    MovePen(0.7, winHeight - 0.7 - GetFontHeight() * 11);
    DrawTextString("剩余血量:");

    DrawTextString(IntToString(health));
}

//绘制多边形
void drawPolygon(point* p, int n) {
    SetPointSize(15);
    MovePen(p->x, p->y);
    for(int i = 0; i < n - 1; i++) {
        DrawLine((p+i+1)->x - (p+i)->x, (p+i+1)->y - (p+i)->y);
    }
    DrawLine(p->x - (p+n-1)->x, p->y - (p+n-1)->y);
}

//绘制一个格子
void drawGrid(double x, double y) {
    SetPointSize(15);
    MovePen(x - GRIDSIZE / 2, y - GRIDSIZE / 2);
    DrawLine(0, GRIDSIZE);
    DrawLine(GRIDSIZE, 0);
    DrawLine(0, -GRIDSIZE);
    DrawLine(-GRIDSIZE, 0);
}

//绘制一扇门
void drawDoor(double x, double y) {

    SetPointSize(40);

    MovePen(x - GRIDSIZE / 2 + 0.05, y - GRIDSIZE / 2 + 0.05);
    DrawLine(0, GRIDSIZE - 0.1);
    DrawLine(GRIDSIZE - 0.1, 0);
    DrawLine(0, 0.1 - GRIDSIZE);
    DrawLine(0.1 - GRIDSIZE, 0);
    MovePen(x - GRIDSIZE / 2 + 0.05, y);
    DrawLine(GRIDSIZE - 0.1, 0);
    MovePen(x, y - GRIDSIZE / 2 + 0.05);
    DrawLine(0, GRIDSIZE - 0.1);

}

//绘制一把钥匙
void drawKey(double x, double y) {

    SetPointSize(25);

    MovePen(x - 0.08, y - GRIDSIZE / 2 + 0.18);
    DrawLine(0.08, -0.08);

    MovePen(x - 0.08, y - GRIDSIZE / 2 + 0.12);
    DrawLine(0.08, -0.08);
    DrawLine(0, GRIDSIZE - 0.2);

    MovePen(x, y + GRIDSIZE / 2 - 0.16);

    for(double r = 0.01; r <= 0.07; r += 0.005) {
        DrawArc(r, 270, 360);
    }

}

//绘制地图选择界面
void drawChooseMaze() {

    SetPointSize(15);

    double h = GetFontHeight() * 1.5;
    double w = TextStringWidth("编辑") * 2;
    double wlist = TextStringWidth("MaZheDaShuaiGe")*1.2;

    int n = fileCount(fileName);
    string* fileNameList = (string*) malloc((n + 1) * sizeof(string));
    fileNameList[0] = "地图列表";
    FileNode* p = fileName;

    for(int i = 1; i <= n; i++) {

        p = p->next;
        fileNameList[i] = p->name;

    }

    int selection;
    selection = menuList(GenUIID(0), w-0.05, winHeight-2*h, wlist, wlist, h, fileNameList, n+1);
    if(selection > 0) {
        strcpy(name, readMaze(selection));
        isHelping = 0;
        isAbouting = 0;
        isOpening = 0;
        isEditing = 0;
        isAnswering = 0;
        isNexting = 0;
        isShowing = 0;
        win = 0;
        leftKey = 0;
        hasBegin = 0;
        health = 100;
    }

}

//绘制元素选择界面
void drawChooseElement() {

    SetPointSize(15);

    double h = GetFontHeight() * 1.5;
    double w = TextStringWidth("编辑") * 2;
    double wlist = TextStringWidth("MaZheDaShuaiGe")*1.2;

    static string elementList[] = {
            "元素列表",
            "道路",
            "障碍物",
            "起点",
            "终点",
            "门",
            "钥匙"
    };

    int selection;

    selection = menuList(GenUIID(0), w-0.05, winHeight-2*h, wlist, wlist, h, elementList, sizeof(elementList)/sizeof(elementList[0]));
    if(selection > 0) {

        isChoosingElement = 0;
        element = selection - 1;

    }

}

//显示当前元素
void drawCurrElement() {

    SetPointSize(15);

    SetPenColor("Blue");

    double h = GetFontHeight() * 1.5;
    double w = TextStringWidth("编辑") * 2;
    double wlist = TextStringWidth("MaZheDaShuaiGe")*1.2;

    static string elementList[] = {
            "道路",
            "障碍物",
            "起点",
            "终点",
            "门",
            "钥匙"
    };

    MovePen(w+wlist+0.05, winHeight-2*h);
    DrawTextString("编辑中,当前元素为:");
    DrawTextString(elementList[element]);

}

//绘制菜单栏
void drawMenu() {

    static string menuListFile[] = {
            "文件",
            "新建地图 | Ctrl-N",
            "打开地图 | Ctrl-O",
            "保存地图 | Ctrl-S",
            "退出 | Ctrl-T"
    };

    static string menuListEdit[] = {
            "编辑",
            "随机生成 | Ctrl-L",
            "手动编辑 | Ctrl-P",
            "元素选择"
    };

    static string menuListAnswer[] = {
            "求解",
            "手动求解 | Ctrl-Q",
            "提示一步 | Ctrl-W",
            "程序求解 | Ctrl-E",
            "自动执行",
            "求解过程"
    };

    static string menuListHelp[] = {
            "帮助",
            "使用说明",
            "关于"
    };

    SetPointSize(15);

    double h = GetFontHeight() * 1.5;
    double w = TextStringWidth(menuListEdit[0]) * 2;
    double wlist = TextStringWidth(menuListFile[3])*1.2;
    int selection;

    if(button(GenUIID(0), w*6+0.4, winHeight-2*h, w, h, isRenaming ? "确认" : "重命名")) {
        isRenaming = !isRenaming;
    }

    if(isRenaming) textbox(GenUIID(0), w*7+0.5, winHeight-2*h, 20*TextStringWidth("W"), h, name, 21);
    else drawLabel(w*7+0.5, winHeight-2*h+0.06, name);

    drawMenuBar(0, winHeight - h, winWidth, h);

    //FILE
    selection = menuList(GenUIID(0), 0, winHeight-h, w, wlist, h, menuListFile, sizeof(menuListFile)/sizeof(menuListFile[0]));
    if(selection == 1) {

        //新建地图
        isHelping = 0;
        isAbouting = 0;
        isEditing = 0;
        isAnswering = 0;
        isNexting = 0;
        isShowing = 0;
        win = 0;
        hasBegin = 0;
        health = 100;
        playerX = 1;
        playerY = MAZESIZE - 2;
        targetX = MAZESIZE - 2;
        targetY = 1;

        memset(maze, 0, sizeof(maze));
        for(int i = 0; i < MAZESIZE; i++) {
            maze[i][0] = 1;
            maze[0][i] = 1;
            maze[MAZESIZE-1][i] = 1;
            maze[i][MAZESIZE-1] = 1;
        }

    }else if(selection == 2) {

        //打开地图
        isOpening = 1;
        isChoosingElement = 0;

    }else if(selection == 3) {

        //保存地图
        if(!win) {
            FILE *fp;
            if (filePush(fileName, name)) {

                fp = fopen("FileName.txt", "a+");
                fprintf(fp, "%s\n", name);
                fclose(fp);

            }

            string file = (string) malloc(32 * sizeof(char));
            strcpy(file, "./MazeList/");
            strcat(file, name);

            fp = fopen(file, "w");
            fprintf(fp, "%d %d\n", playerX, playerY);
            fprintf(fp, "%d %d\n", targetX, targetY);

            for (int i = 0; i < MAZESIZE; i++) {
                for (int j = 0; j < MAZESIZE; j++) {
                    fprintf(fp, "%d ", maze[i][j]);
                }
                fprintf(fp, "\n");
            }

            fclose(fp);

        }


    }else if(selection == 4) {

        //退出
        exit(-1);

    }

    //Edit
    selection = menuList(GenUIID(0), w, winHeight-h, w, wlist, h, menuListEdit, sizeof(menuListEdit)/sizeof(menuListEdit[0]));
    if(selection == 1) {

        //随机生成
        isHelping = 0;
        isAbouting = 0;
        isEditing = 0;
        isAnswering = 0;
        isNexting = 0;
        isShowing = 0;
        win = 0;
        hasDoor = 0;
        hasKey = 0;
        hasBegin = 0;
        health = 100;

        initMaze();
        while(hasAns() <= 30) initMaze();

    }else if(selection == 2) {

        //手动编辑
        isEditing = !isEditing;
        hasBegin = 0;
        health = 100;

    }else if(selection == 3) {

        //元素选择
        isChoosingElement = !isChoosingElement;
        isOpening = 0;

    }

    //Answer
    selection = menuList(GenUIID(0), 2*w, winHeight-h, w, wlist, h, menuListAnswer, sizeof(menuListAnswer)/sizeof(menuListAnswer[0]));
    if(selection == 1) {

        //手动求解
        isAnswering = 0;
        isNexting = 0;
        isPlaying = 0;
        isShowing = 0;

    }else if(selection == 2) {

        //提示一步
        isNexting = 1;
        isAnswering = 0;

    }else if(selection == 3) {

        //程序求解
        if(!isEditing) {
            isAnswering = 1;
            isNexting = 0;
        }

    }else if(selection == 4) {

        //自动执行
        isShowing = 0;
        isPlaying = !isPlaying;
        isAnswering = isPlaying;
        hasBegin = 0;

    }else if(selection == 5) {

        //求解过程
        isShowing = !isShowing;
        isAnswering = 0;
        isPlaying = 0;
        fakeLeftKey = leftKey;
        for(int i = 0; i < MAZESIZE; i++) {
            for(int j = 0; j < MAZESIZE; j++) {
                fakeMaze[i][j] = maze[i][j];
            }
        }

        show = (queue*) malloc(sizeof(queue));
        show->next = (queue*) malloc(sizeof(queue));
        show->next->x = playerX;
        show->next->y = playerY;
        show->next->next = NULL;

        memset(showVisited, 0, sizeof(showVisited));

        showVisited[playerX][playerY] = 1;

    }

    //Help
    selection = menuList(GenUIID(0), 3*w, winHeight-h, w, wlist, h, menuListHelp, sizeof(menuListHelp)/sizeof(menuListHelp[0]));
    if(selection == 1) {

        //使用说明
        isAbouting = 0;
        isHelping = 1;
        hasBegin = 0;

    }else if(selection == 2) {

        //关于
        isHelping = 0;
        isAbouting = 1;
        hasBegin = 0;

    }

    SetPointSize(15);

}

//初始化界面
void initMap() {
    point map[4] = {
            0.6, 0.1,
            0.6, winHeight - 0.6,
            winWidth - 0.6, winHeight - 0.6,
            winWidth - 0.6, 0.1
    };

    SetPenColor("Orange");
    SetPenSize(3);
    drawPolygon(map, 4);
    SetPenSize(2);
}

//绘制一个点（玩家or终点）
void drawPoint(double x, double y) {
    SetPenSize(2);
    MovePen(x + 0.14, y);
    for(double r = 0.01; r <= 0.14; r += 0.005) {
        DrawArc(r, 0, 360);
    }
}

//绘制迷宫
void drawMaze() {

    SetPointSize(15);
    for(int i = 0; i < MAZESIZE; i++) {
        for(int j = 0; j < MAZESIZE; j++) {
            if(maze[i][j] == 1) {

                //障碍物
                SetPenColor("Black");
                drawGrid(mazePoint[i][j].x, mazePoint[i][j].y);
            }else if(maze[i][j] == 4) {

                //门
                SetPenColor("Dark gray");
                drawDoor(mazePoint[i][j].x, mazePoint[i][j].y);

            }else if(maze[i][j] == 5) {

                //钥匙
                SetPenColor("Magenta");
                drawKey(mazePoint[i][j].x, mazePoint[i][j].y);

            }
        }
    }

}

//绘制
void initStart() {

    if(!hasDoor) setADoor();
    if(!hasKey) setAKey();

    if (isNexting) {
        if (!win) {
            if (hasAns()) {
                queue *way = path;
                while (way->next) way = way->next;
                SetPenColor("Green");
                int px = way->x, py = way->y;
                drawPoint(mazePoint[px][py].x, mazePoint[px][py].y);
            }
        }
    }

    if (isAnswering) {
        if (!win) {
            if (hasAns()) {

                queue *way = path->next;

                while (way->next) {
                    way = way->next;
                    SetPenColor("Green");
                    int px = way->x, py = way->y;
                    drawPoint(mazePoint[px][py].x, mazePoint[px][py].y);
                }
            }
        }
    }

    drawMaze();
    SetPointSize(15);
    SetPenColor("Blue");
    drawPoint(mazePoint[playerX][playerY].x, mazePoint[playerX][playerY].y);
    SetPenColor("Red");
    drawPoint(mazePoint[targetX][targetY].x, mazePoint[targetX][targetY].y);

}

//绘制胜利画面
void drawWinMap() {
    SetPointSize(15);
    SetPenColor("Red");
    string text1 = "成功^_^";
    string text2 = "你可以点击随机生成或新建地图来重新开始";
    string text3 = "或者直接打开已保存的地图";

    SetPointSize(50);
    MovePen(winWidth / 2 - TextStringWidth(text1) / 2, winHeight / 2);
    DrawTextString(text1);
    MovePen(winWidth / 2 - TextStringWidth(text2) / 2, winHeight / 2 - GetFontHeight());
    DrawTextString(text2);
    MovePen(winWidth / 2 - TextStringWidth(text3) / 2, winHeight / 2 - 2 * GetFontHeight());
    DrawTextString(text3);
}

//绘制失败界面
void drawLoseMap() {
    SetPointSize(15);
    SetPenColor("Red");
    string text1 = "失败了T_T";
    string text2 = "你可以点击随机生成或新建地图来重新开始";
    string text3 = "或者直接打开已保存的地图";

    SetPointSize(50);
    MovePen(winWidth / 2 - TextStringWidth(text1) / 2, winHeight / 2);
    DrawTextString(text1);
    MovePen(winWidth / 2 - TextStringWidth(text2) / 2, winHeight / 2 - GetFontHeight());
    DrawTextString(text2);
    MovePen(winWidth / 2 - TextStringWidth(text3) / 2, winHeight / 2 - 2 * GetFontHeight());
    DrawTextString(text3);
}

//绘制帮助界面
void drawHelpMap() {

    SetPointSize(20);
    SetPenColor("Red");

    string ss[] = {

            "蓝色小球为玩家位置 红色小球为终点位置",
            "通过WASD或者方向键来操控小球抵达终点即胜利",
            "初始生命值为100 会在1分钟内降为0",
            "途中会有门阻挡 需要寻找钥匙来开门",
            "钥匙是紫色 门是灰色",
            "地图可以随机生成 也可以自主编辑 随机生成默认1门1钥匙",
            "编辑时可选择想要放置的物品种类",
            "还可以更改地图名字",
            "通过名字来保存或者打开地图",
            "当解密出现困难时可以使用求解中的功能",
            "其中程序解密会可视化给出解密路径",
            "自动执行会在给出路径的同时自动移动完成解密",
            "进入各种编辑模式后均可通过ESC来退出",

    };

    for(int i = 0; i < 13; i++) {

        MovePen(winWidth / 2 - TextStringWidth(ss[i]) / 2, winHeight - (i + 3) * (GetFontHeight() + 0.2));
        DrawTextString(ss[i]);

    }
}

//绘制关于界面
void drawAboutMap() {

    SetPointSize(50);
    SetPenColor("Gray");

    string ss[] = {

            "2022-2023春夏学期",
            "程序设计专题大作业",
            "《迷宫游戏》",
            "作者:电信2201朱禹同",
            "联系方式:18905867535",
            "邮箱:1210600514@qq.com"

    };

    for(int i = 0; i < 6; i++) {

        MovePen(winWidth / 2 - TextStringWidth(ss[i]) / 2, winHeight - (i + 2) * (GetFontHeight() + 0.3));
        DrawTextString(ss[i]);

    }
}

//刷新
void display() {
    DisplayClear();

    if(playerX == targetX && playerY == targetY) win = 1;

    if(isHelping) {

        drawHelpMap();

    }else if(isAbouting) {

        drawAboutMap();

    }else if(win) {

        isNexting = 0;
        isAnswering = 0;
        if(win == 1) drawWinMap();
        else if(win == -1) drawLoseMap();
    }else{

        initMap();
        initStart();
        drawLeftKeys();
        drawHealth();

    }

    if(isOpening) {
        drawChooseMaze();
    }

    if(isChoosingElement) {
        drawChooseElement();
    }

    if(isEditing) {
        drawCurrElement();
    }

    drawMenu();

}