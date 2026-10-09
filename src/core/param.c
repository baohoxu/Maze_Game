#include "param.h"

/* ---------------- 来自原 main.c 的定义 ---------------- */

int    win = 0;
int    playerX = 1, playerY = MAZESIZE - 2;
int    targetX = MAZESIZE - 2, targetY = 1;
double winWidth = 10.0, winHeight = 7.0;
int    maze[MAZESIZE][MAZESIZE];
point  mazePoint[MAZESIZE][MAZESIZE];
double mouseX, mouseY;
Path  *path = NULL;
int    isAnswering = 0;
int    isNexting = 0;
int    isEditing = 0;
FileNode *fileName = NULL;

/* ---------------- 来自原 maze.c 文件作用域的定义 ---------------- */

int  hasBegin = 0;
int  health = 100;
int  hasDoor = 0;
int  hasKey = 0;
int  leftKey = 0;
int  fakeLeftKey = 0;
int  element = 1;
int  isChoosingElement = 0;
int  isOpening = 0;
int  isRenaming = 0;
int  isPlaying = 0;
int  isHelping = 0;
int  isAbouting = 0;
int  isShowing = 0;
char name[21] = "未命名";

Path *show;
int   showVisited[MAZESIZE][MAZESIZE];
int   fakeMaze[MAZESIZE][MAZESIZE];

void param_init(void) {
    win = 0;
    playerX = 1;
    playerY = MAZESIZE - 2;
    targetX = MAZESIZE - 2;
    targetY = 1;
    winWidth = 10.0;
    winHeight = 7.0;
    mouseX = 0.0;
    mouseY = 0.0;
    isAnswering = 0;
    isNexting = 0;
    isEditing = 0;
    fileName = NULL;

    hasBegin = 0;
    health = 100;
    hasDoor = 0;
    hasKey = 0;
    leftKey = 0;
    fakeLeftKey = 0;
    element = 1;
    isChoosingElement = 0;
    isOpening = 0;
    isRenaming = 0;
    isPlaying = 0;
    isHelping = 0;
    isAbouting = 0;
    isShowing = 0;
    name[0] = '\0';
    strcpy(name, "未命名");

    /*
     * path / show 指向动态分配的链表，不在这里分配，
     * 由调用方在需要时创建（与原 main.c 的行为一致）。
     */
}
