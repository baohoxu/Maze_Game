#ifndef GRAPHICSPROJECT_QUEUE_H
#define GRAPHICSPROJECT_QUEUE_H
#include <stdlib.h>

typedef struct ListNode {
    int x, y;
    struct ListNode* next;
    struct ListNode* prev;
} Path;

void push(Path* head, Path* prev, int x, int y);

void pop(Path* head);

void copy(Path* a, Path* b);

Path* reverse(Path* head);

int count(Path* head);

#endif //GRAPHICSPROJECT_QUEUE_H
