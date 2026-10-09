#ifndef GRAPHICSPROJECT_PATHFIND_H
#define GRAPHICSPROJECT_PATHFIND_H

/*
 * File: pathfind.h
 * -----------------
 * 迷宫求解：广度优先搜索与可达性判定。
 *
 * 注意：bfs 的参数 visited 是 int[MAZESIZE][MAZESIZE] 退化成的 int**，
 * 由调用方（hasAns）通过指针数组构造，语义与 maze 一致（[row][col]）。
 */

#include "param.h"

void bfs(Path* p, int** visited, int steps, int* ret, int target);
int hasAns(void);

#endif //GRAPHICSPROJECT_PATHFIND_H
