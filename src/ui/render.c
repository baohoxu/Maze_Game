#include "render.h"
#include "maze_model.h"
#include "pathfind.h"

/* ===== 基础图元绘制 ===== */

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
                Path *way = path;
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

                Path *way = path->next;

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
