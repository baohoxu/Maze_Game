#ifndef GRAPHICSPROJECT_APP_H
#define GRAPHICSPROJECT_APP_H

/*
 * File: app.h
 * ------------
 * 应用层的公共依赖与主循环接口。
 *
 * 本文件只负责两件事：
 *   1. 引入 libgraphics 与 Win32 的头文件
 *   2. 声明应用级入口（Main / display / TimerEventProcess）
 *
 * 它**不**引入 core 层的实现细节（queue.h / file.h / param.h），
 * 各模块按需自行 include。
 */

#include <math.h>
#include <windows.h>
#include <winuser.h>
#include <mmsystem.h>

#include "graphics.h"
#include "extgraph.h"
#include "imgui.h"
#include "types.h"

#pragma comment(lib, "WinMM.Lib")

/* 定时器 ID，与 Main 中的 startTimer 调用对应 */
enum timer{
    FLUSH,
    PLAY,
    HEALTH,
    ANSWER
};

void TimerEventProcess(int TimerID);

void display(void);

#endif //GRAPHICSPROJECT_APP_H
