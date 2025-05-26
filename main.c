#include "maze.h"

int win = 0;

int playerX = 1, playerY = MAZESIZE - 2;

int targetX = MAZESIZE - 2, targetY = 1;

double winWidth = 10.0, winHeight = 7.0;

int maze[MAZESIZE][MAZESIZE];

point mazePoint[MAZESIZE][MAZESIZE];

double mouseX, mouseY;

queue* path = NULL;

int isAnswering = 0;

int isNexting = 0;

int isEditing = 0;

FileNode* fileName = NULL;

void Main() {

    PlaySound("./Music/魔术先生.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);

    path = (queue*)malloc(sizeof(queue));
    path->next = NULL;
    SetWindowTitle("迷宫游戏");
    SetWindowSize(winWidth, winHeight);
    InitGraphics();

    SetFont("Times");
    SetPointSize(15);
    readFileName();

    initMazePoint();

    registerTimerEvent(TimerEventProcess);
    registerMouseEvent(MouseEventProcess);
    registerCharEvent(CharEventProcess);
    registerKeyboardEvent(KeyboardEventProcess);

    startTimer(FLUSH, 25);
    startTimer(PLAY, 250);
    startTimer(HEALTH, 600);
    startTimer(ANSWER, 1000);

}