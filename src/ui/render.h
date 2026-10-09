#ifndef GRAPHICSPROJECT_RENDER_H
#define GRAPHICSPROJECT_RENDER_H

/*
 * File: render.h
 * --------------
 * 基础图元绘制与迷宫主体渲染。
 * 只读取全局状态（param.h），不修改状态。
 */

#include "app.h"
#include "param.h"

void drawGrid(double x, double y);
void drawDoor(double x, double y);
void drawKey(double x, double y);
void drawPolygon(point* p, int n);
void drawPoint(double x, double y);
void drawMaze(void);
void initMap(void);
void initStart(void);
void drawLeftKeys(void);
void drawHealth(void);
void drawWinMap(void);
void drawLoseMap(void);
void drawHelpMap(void);
void drawAboutMap(void);

/* 整数转字符串（自造实现，等价于 strlib.h 的 IntegerToString） */
string IntToString(int n);

#endif //GRAPHICSPROJECT_RENDER_H
