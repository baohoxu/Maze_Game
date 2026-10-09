#ifndef GRAPHICSPROJECT_TYPES_H
#define GRAPHICSPROJECT_TYPES_H

/*
 * File: types.h
 * -------------
 * 全局基础类型与常量。本文件**不允许** include 项目内任何其他头文件，
 * 以保证它位于依赖图的最底层：ui -> core -> common。
 */

/* 迷宫边长（格子数），含四周的一圈围墙 */
#define MAZESIZE 14

/* 单个格子的边长（英寸） */
#define GRIDSIZE 0.4

/* 迷宫格子类型。数值与 MazeList/ 下的地图文件格式一致 */
typedef enum {
    CELL_ROAD = 0,      /* 道路，可通行 */
    CELL_WALL = 1,      /* 障碍物，不可通行 */
    CELL_DOOR = 4,      /* 门，持有钥匙时可通行 */
    CELL_KEY  = 5       /* 钥匙，拾取后 keys_left + 1 */
} CellType;

/* 屏幕坐标（单位为英寸，原点在绘图窗口左下角） */
typedef struct {
    double x, y;
} point;

/*
 * 格子索引。
 * 注意：与 maze[row][col] 对应，row 是行（视觉上的竖直方向），
 * col 是列（视觉上的水平方向）。与 MazeList/ 文件的行列顺序一致。
 */
typedef struct {
    int row, col;
} Cell;

#endif //GRAPHICSPROJECT_TYPES_H
