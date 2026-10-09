#include "app.h"
#include "param.h"
#include "render.h"
#include "menu.h"
#include "pathfind.h"

void TimerEventProcess(int TimerID) {

    if(TimerID == HEALTH && hasBegin) {
        --health;
        if(!health) win = -1;
    }

    if(TimerID == FLUSH) display();

    if(TimerID == PLAY && isPlaying) {

        hasAns();
        path = reverse(path);

        playerX = path->x;
        playerY = path->y;

        if(maze[playerX][playerY] == 4 && leftKey) {
            --leftKey;
            maze[playerX][playerY] = 0;
        }else if(maze[playerX][playerY] == 5) {
            ++leftKey;
            maze[playerX][playerY] = 0;
        }

        if(playerX == targetX && playerY == targetY) isPlaying = 0;

    }

    if(TimerID == ANSWER && isShowing) {

        int k = count(show);
        if(!k) return;

        int x, y;

        for(int i = 0; i < k; i++) {
            x = show->next->x;
            y = show->next->y;

            if(fakeMaze[x][y] == 4 && fakeLeftKey || fakeMaze[x][y] == 5) {

                memset(showVisited, 0, sizeof(showVisited));
                if(fakeMaze[x][y] == 5) {
                    ++fakeLeftKey;
                    fakeMaze[x][y] = 0;
                }else{
                    --fakeLeftKey;
                    fakeMaze[x][y] = 0;
                }

            }

            if(x == targetX && y == targetY) isShowing = 0;

            if((!fakeMaze[x+1][y] || fakeMaze[x+1][y] == 5 || fakeMaze[x+1][y] == 4 && fakeLeftKey) && !showVisited[x+1][y]) {
                push(show, show->next, x+1, y);
                showVisited[x+1][y] = 1;
            }

            if((!fakeMaze[x][y+1] || fakeMaze[x][y+1] == 5 || fakeMaze[x][y+1] == 4 && fakeLeftKey) && !showVisited[x][y+1]) {
                push(show, show->next, x, y+1);
                showVisited[x][y+1] = 1;
            }

            if((!fakeMaze[x-1][y] || fakeMaze[x-1][y] == 5 || fakeMaze[x-1][y] == 4 && fakeLeftKey) && !showVisited[x-1][y]) {
                push(show, show->next, x-1, y);
                showVisited[x-1][y] = 1;
            }

            if((!fakeMaze[x][y-1] || fakeMaze[x][y-1] == 5 || fakeMaze[x][y-1] == 4 && fakeLeftKey) && !showVisited[x][y-1]) {
                push(show, show->next, x, y-1);
                showVisited[x][y-1] = 1;
            }

            pop(show);

        }

    }

    if(isShowing) {

        int k = count(show);
        if(!k) return;

        Path* p = show;

        int x, y;

        SetPenColor("Green");

        for(int i = 0; i < k; i++) {

            x = p->next->x;
            y = p->next->y;

            drawPoint(mazePoint[x][y].x, mazePoint[x][y].y);

            p = p->next;

        }
    }
}

void display() {
    DisplayClear();

    if(playerX == targetX && playerY == targetY) win = 1;

    if(isHelping) {

        drawHelpMap();

    }else if(isAbouting) {

        drawAboutMap();

    }else if(win) {

        isNexting = 0;
        isAnswering = 0;
        if(win == 1) drawWinMap();
        else if(win == -1) drawLoseMap();
    }else{

        initMap();
        initStart();
        drawLeftKeys();
        drawHealth();

    }

    if(isOpening) {
        drawChooseMaze();
    }

    if(isChoosingElement) {
        drawChooseElement();
    }

    if(isEditing) {
        drawCurrElement();
    }

    drawMenu();

}
