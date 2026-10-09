#ifndef GRAPHICSPROJECT_INPUT_H
#define GRAPHICSPROJECT_INPUT_H

/*
 * File: input.h
 * --------------
 * 键盘 / 字符 / 鼠标事件的回调入口。
 * 这三个函数由 libgraphics 在事件发生时调用（见 Main 中的 register*Event）。
 */

#include "app.h"
#include "param.h"

void CharEventProcess(char ch);
void KeyboardEventProcess(int key, int event);
void MouseEventProcess(int x, int y, int button, int event);

#endif //GRAPHICSPROJECT_INPUT_H
