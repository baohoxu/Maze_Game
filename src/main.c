#include "app.h"
#include "param.h"
#include "input.h"
#include "render.h"
#include "pathfind.h"
#include "maze_model.h"

/*
 * 程序入口。
 *
 * 注意：真正的 Win32 入口 WinMain 定义在 libgraphics/graphics.c 中，
 * 它在窗口创建完成后回调本文件的 Main()。因此这里没有 int main()。
 */
void Main() {

    PlaySound("./Music/魔术先生.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);

    param_init();

    path = (Path*)malloc(sizeof(Path));
    path->next = NULL;
    path->prev = NULL;

    SetWindowTitle("迷宫游戏");
    SetWindowSize(winWidth, winHeight);
    InitGraphics();

    SetFont("Times");
    SetPointSize(15);
    readFileName(&fileName);

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
