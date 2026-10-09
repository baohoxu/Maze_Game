#include "input.h"

void CharEventProcess(char ch) {
    uiGetChar(ch);
    display();
}

//键盘控制
void KeyboardEventProcess(int key, int event) {
    uiGetKeyboard(key,event);

    if(event == BUTTON_DOWN && key == VK_ESCAPE) {

        isChoosingElement = 0;
        isOpening = 0;
        isPlaying = 0;
        isHelping = 0;
        isAbouting = 0;
        isAnswering = 0;
        isNexting = 0;
        isEditing = 0;
        isShowing = 0;

    }

    if(event == BUTTON_DOWN && key == VK_RETURN) isRenaming = !isRenaming;

    if(event == BUTTON_DOWN && !win && !isRenaming) {
        if(!isEditing) {

            if (key == 'W' || key == VK_UP) {
                if (!maze[playerX][playerY+1]) playerY += 1;
                else if (maze[playerX][playerY+1] == 4) {
                    if(leftKey) {
                        --leftKey;
                        playerY += 1;
                        maze[playerX][playerY] = 0;
                    }
                }else if (maze[playerX][playerY+1] == 5) {
                    ++leftKey;
                    playerY += 1;
                    maze[playerX][playerY] = 0;
                }
                isNexting = 0;
                hasBegin = 1;
            }

            if (key == 'D' || key == VK_RIGHT) {
                if (!maze[playerX+1][playerY]) playerX += 1;
                else if (maze[playerX+1][playerY] == 4) {
                    if(leftKey) {
                        --leftKey;
                        playerX += 1;
                        maze[playerX][playerY] = 0;
                    }
                }else if (maze[playerX+1][playerY] == 5) {
                    ++leftKey;
                    playerX += 1;
                    maze[playerX][playerY] = 0;
                }
                isNexting = 0;
                hasBegin = 1;
            }

            if (key == 'S' || key == VK_DOWN) {
                if (!maze[playerX][playerY-1]) playerY -= 1;
                else if (maze[playerX][playerY-1] == 4) {
                    if(leftKey) {
                        --leftKey;
                        playerY -= 1;
                        maze[playerX][playerY] = 0;
                    }
                }else if (maze[playerX][playerY-1] == 5) {
                    ++leftKey;
                    playerY -= 1;
                    maze[playerX][playerY] = 0;
                }
                isNexting = 0;
                hasBegin = 1;
            }

            if (key == 'A' || key == VK_LEFT) {
                if (!maze[playerX-1][playerY]) playerX -= 1;
                else if (maze[playerX-1][playerY] == 4) {
                    if(leftKey) {
                        --leftKey;
                        playerX -= 1;
                        maze[playerX][playerY] = 0;
                    }
                }else if (maze[playerX-1][playerY] == 5) {
                    ++leftKey;
                    playerX -= 1;
                    maze[playerX][playerY] = 0;
                }
                isNexting = 0;
                hasBegin = 1;
            }
        }
    }

}

//鼠标控制
void MouseEventProcess(int x, int y, int button, int event) {
    uiGetMouse(x, y, button, event);

    mouseX = ScaleXInches(x);
    mouseY = ScaleYInches(y);

    if(button == LEFT_BUTTON && event == BUTTON_DOWN) {
        if (isEditing) {
            for (int i = 1; i < MAZESIZE - 1; i++) {
                for (int j = 1; j < MAZESIZE - 1; j++) {
                    if (fabs(mouseX - mazePoint[i][j].x) <= GRIDSIZE / 2 && fabs(mouseY - mazePoint[i][j].y) <= GRIDSIZE / 2) {
                        if(element == 2 && !maze[i][j] && !(i == targetX && j == targetY)) {
                            playerX = i;
                            playerY = j;
                        }else if(element == 3 && !maze[i][j] && !(i == playerX && j == playerY)) {
                            targetX = i;
                            targetY = j;
                        }else if(!(i == playerX && j == playerY) && !(i == targetX && j == targetY))
                            maze[i][j] = element;

                        i = MAZESIZE;
                        break;
                    }
                }
            }
        }
    }

}
