#ifndef GRAPHICSPROJECT_MAZE_H
#define GRAPHICSPROJECT_MAZE_H

#include <math.h>
#include <windows.h>
#include <winuser.h>
#include <mmsystem.h>

#include "graphics.h"
#include "extgraph.h"
#include "imgui.h"
#include "queue.h"
#include "file.h"

#pragma comment(lib, "WinMM.Lib")

#define GRIDSIZE 0.4
#define MAZESIZE 14

enum timer{
    FLUSH,
    PLAY,
    HEALTH,
    ANSWER
};

typedef struct {
    double x, y;
}point;

void TimerEventProcess(int TimerID);

void CharEventProcess(char ch);

void KeyboardEventProcess(int key, int event);

void MouseEventProcess(int x, int y, int button, int event);

string IntToString(int n);

void bfs(queue* p, int** visited, int steps, int* ret, int target);

int hasAns();

void initMaze();

void initMazePoint();

void setADoor();

void setAKey();

void drawLeftKeys();

void drawHealth();

void drawPolygon(point* p, int n);

void drawGrid(double x, double y);

void drawDoor(double x, double y);

void drawKey(double x, double y);

void drawChooseMaze();

void drawChooseElement();

void drawCurrElement();

void drawMenu();

void initMap();

void drawPoint(double x, double y);

void drawMaze();

void initStart();

void drawWinMap();

void drawLoseMap();

void drawHelpMap();

void drawAboutMap();

void display();

#endif //GRAPHICSPROJECT_MAZE_H
