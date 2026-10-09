#ifndef GRAPHICSPROJECT_FILE_H
#define GRAPHICSPROJECT_FILE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"

typedef char *string;

typedef struct LinkNode {
    char name[21];
    struct LinkNode* next;
}FileNode;

void readFileName(FileNode** fileName);

int fileCount(FileNode* head);

int filePush(FileNode* head, string name);

string readMaze(int n, FileNode* fileName, 
                int* playerX, int* playerY, 
                int* targetX, int* targetY, 
                int maze[][MAZESIZE]);

#endif //GRAPHICSPROJECT_FILE_H