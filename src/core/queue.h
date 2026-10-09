#ifndef GRAPHICSPROJECT_QUEUE_H
#define GRAPHICSPROJECT_QUEUE_H
#include <stdlib.h>

typedef struct ListNode {
    int x, y;
    struct ListNode* next;
    struct ListNode* prev;
}queue;

void push(queue* head, queue* prev, int x, int y);

void pop(queue* head);

void copy(queue* a, queue* b);

queue* reverse(queue* head);

int count(queue* head);

#endif //GRAPHICSPROJECT_QUEUE_H
