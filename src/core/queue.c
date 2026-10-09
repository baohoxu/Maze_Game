#include "queue.h"

void push(Path* head, Path* prev, int x, int y) {
    while(head->next) head = head->next;
    head->next = (Path*) malloc(sizeof(Path));

    head = head->next;

    head->prev = prev;
    head->next = NULL;
    head->x = x;
    head->y = y;
}

void pop(Path* head) {

    if(!head->next) return;

    head->next = head->next->next;

}

void copy(Path* a, Path* b) {

    //清空a
    while(a->next) {
        pop(a);
    }

    //将b复制入a
    while(b->prev) {
        push(a, NULL, b->x, b->y);
        b = b->prev;
    }
}

Path* reverse(Path* head) {
    if(!head || !head->next) return head;

    Path* newhead = reverse(head->next);
    head->next->next = head;
    head->next = NULL;
    return newhead;
}

int count(Path* head) {
    int ret = 0;
    while(head->next) {
        ++ret;
        head = head->next;
    }
    return ret;
}