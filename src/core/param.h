#ifndef GRAPHICSPROJECT_PARAM_H
#define GRAPHICSPROJECT_PARAM_H

/*
 * File: param.h
 * -------------
 * 全局可变状态的**唯一**归属地。
 *
 * 约定：本文件是依赖图里的"汇聚点"，允许 include core 层的头文件
 * （types.h / queue.h / file.h）。它自身不定义任何函数逻辑，
 * 只声明状态与初始化入口。
 *
 * 任何 .c 文件需要访问游戏状态时，一律 #include "param.h"，
 * 不允许再写 extern 声明。
 */

#include "types.h"
#include "queue.h"
#include "file.h"

/* ---------------- 来自原 main.c ---------------- */

extern int    win;                      /* 0 进行中 / 1 胜利 / -1 失败 */
extern int    playerX, playerY;
extern int    targetX, targetY;
extern double winWidth, winHeight;
extern int    maze[MAZESIZE][MAZESIZE];
extern point  mazePoint[MAZESIZE][MAZESIZE];
extern double mouseX, mouseY;
extern Path  *path;                     /* BFS 求出的路径 */
extern int    isAnswering;
extern int    isNexting;
extern int    isEditing;
extern FileNode *fileName;              /* 地图文件名链表（哑头节点） */

/* ---------------- 来自原 maze.c 文件作用域 ---------------- */

extern int  hasBegin;
extern int  health;
extern int  hasDoor;
extern int  hasKey;
extern int  leftKey;
extern int  fakeLeftKey;
extern int  element;
extern int  isChoosingElement;
extern int  isOpening;
extern int  isRenaming;
extern int  isPlaying;
extern int  isHelping;
extern int  isAbouting;
extern int  isShowing;
extern char name[21];

extern Path *show;
extern int   showVisited[MAZESIZE][MAZESIZE];
extern int   fakeMaze[MAZESIZE][MAZESIZE];

/*
 * Function: param_init
 * --------------------
 * 把上述所有全局变量恢复到"程序刚启动"的状态。
 * 原来这些初值以静态初始化式的形式散落在 main.c 与 maze.c 中，
 * 这里集中实现，语义与原值逐一对齐。
 */
void param_init(void);

#endif //GRAPHICSPROJECT_PARAM_H
