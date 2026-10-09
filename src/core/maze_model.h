#ifndef GRAPHICSPROJECT_MAZE_MODEL_H
#define GRAPHICSPROJECT_MAZE_MODEL_H

/*
 * File: maze_model.h
 * -------------------
 * 迷宫数据的构造与初始化（随机生成、放置门与钥匙、记录格子坐标）。
 * 这些函数会修改 param.h 中的全局状态。
 */

#include "param.h"

void initMaze(void);
void initMazePoint(void);
void setADoor(void);
void setAKey(void);

#endif //GRAPHICSPROJECT_MAZE_MODEL_H
