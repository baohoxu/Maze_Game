#include "pathfind.h"

//广度优先搜索
void bfs(Path* p, int** visited, int steps, int* ret, int target) {

    int k = count(p);
    if(!k) return;

    int x, y;


    for(int i = 0; i < k; i++) {
        x = p->next->x;
        y = p->next->y;

        if(target == 4 && maze[x][y] == 4 && leftKey || target == 5 && maze[x][y] == 5 || !target && x == targetX && y == targetY) {
            copy(path, p->next);
            *ret = steps;
            return;
        }

        if((!maze[x+1][y] || maze[x+1][y] == 5 || maze[x+1][y] == 4 && leftKey) && !visited[x+1][y]) {
            push(p, p->next, x+1, y);
            visited[x+1][y] = 1;
        }

        if((!maze[x][y+1] || maze[x][y+1] == 5 || maze[x][y+1] == 4 && leftKey) && !visited[x][y+1]) {
            push(p, p->next, x, y+1);
            visited[x][y+1] = 1;
        }

        if((!maze[x-1][y] || maze[x-1][y] == 5 || maze[x-1][y] == 4 && leftKey) && !visited[x-1][y]) {
            push(p, p->next, x-1, y);
            visited[x-1][y] = 1;
        }

        if((!maze[x][y-1] || maze[x][y-1] == 5 || maze[x][y-1] == 4 && leftKey) && !visited[x][y-1]) {
            push(p, p->next, x, y-1);
            visited[x][y-1] = 1;
        }

        pop(p);

    }

    bfs(p, visited, steps+1, ret, target);

}

//解迷宫
int hasAns() {

    Path* currPath = (Path*) malloc(sizeof(Path));
    currPath->next = NULL;

    int p[MAZESIZE][MAZESIZE];
    memset(p, 0, sizeof(p));

    int** visited= (int**) malloc(MAZESIZE * sizeof(int*));
    for(int i = 0; i < MAZESIZE; i++) visited[i] = p[i];

    push(currPath, NULL, playerX, playerY);
    visited[playerX][playerY] = 1;
    int ret = 0;
    bfs(currPath, visited, 0, &ret, 0);

    Path *way = path->next;
    int doors = 0;

    if(ret) {
        while (way->next) {
            way = way->next;
            int px = way->x, py = way->y;
            if (maze[px][py] == 4) ++doors;
        }
    }

    if(doors > leftKey) ret = 0;

    if(!ret) {
        memset(p, 0, sizeof(p));
        currPath->next = NULL;
        push(currPath, NULL, playerX, playerY);
        bfs(currPath, visited, 0, &ret, 5);
    }

    if(!ret) {
        memset(p, 0, sizeof(p));
        currPath->next = NULL;
        push(currPath, NULL, playerX, playerY);
        bfs(currPath, visited, 0, &ret, 4);
    }

    return ret;

}
