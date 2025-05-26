#include "file.h"

extern FileNode* fileName;
extern int maze[MAZESIZE][MAZESIZE];

extern int playerX, playerY;
extern int targetX, targetY;

void readFileName() {
    FILE* name = fopen("FileName.txt", "r");

    fileName = (FileNode*) malloc(sizeof(FileNode));
    fileName->next = NULL;
    char ss[20];

    FileNode* p = fileName;
    while(fscanf(name, "%s", ss) != EOF) {
        p->next = (FileNode*) malloc(sizeof(FileNode));
        p = p->next;
        strcpy(p->name, ss);
        p->next = NULL;
    }

    fclose(name);
}

int fileCount(FileNode* head) {

    int ret = 0;
    while(head->next) {
        ++ret;
        head = head->next;
    }

    return ret;
}

int filePush(FileNode* head, string name) {

    while(head->next && strcmp(head->next->name, name) != 0) head = head->next;
    if(head->next) return 0;
    head->next = (FileNode*) malloc(sizeof(FileNode));
    head = head->next;
    head->next = NULL;
    strcpy(head->name, name);

    return 1;
}

string readMaze(int n) {

    FileNode* p = fileName;
    for(int i = 0; i < n; i++) {
        p = p->next;
    }

    string file = (string) malloc(32 * sizeof(char));
    strcpy(file, "./MazeList/");
    strcat(file, p->name);

    FILE* fp = fopen(file, "r");

    fscanf(fp, "%d %d", &playerX, &playerY);
    fscanf(fp, "%d %d", &targetX, &targetY);

    for(int i = 0; i < MAZESIZE; i++) {
        for(int j = 0; j < MAZESIZE; j++) {
            fscanf(fp, "%d", &maze[i][j]);
        }
    }

    fclose(fp);

    free(file);

    return p->name;
}