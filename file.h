#ifndef GRAPHICSPROJECT_FILE_H
#define GRAPHICSPROJECT_FILE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "maze.h"

typedef struct LinkNode {
    char name[21];
    struct LinkNode* next;
}FileNode;

void readFileName();

int fileCount(FileNode* head);

int filePush(FileNode* head, string name);

string readMaze(int n);

#endif //GRAPHICSPROJECT_FILE_H