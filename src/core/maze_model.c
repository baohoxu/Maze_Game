#include "maze_model.h"
#include "pathfind.h"

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

    Path* p = path;
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
