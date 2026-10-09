#include "queue.h"

void push(queue* head, queue* prev, int x, int y) {
    while(head->next) head = head->next;
    head->next = (queue*) malloc(sizeof(queue));

    head = head->next;

    head->prev = prev;
    head->next = NULL;
    head->x = x;
    head->y = y;
}

void pop(queue* head) {

    if(!head->next) return;

    head->next = head->next->next;

}

void copy(queue* a, queue* b) {

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

queue* reverse(queue* head) {
    if(!head || !head->next) return head;

    queue* newhead = reverse(head->next);
    head->next->next = head;
    head->next = NULL;
    return newhead;
}

int count(queue* head) {
    int ret = 0;
    while(head->next) {
        ++ret;
        head = head->next;
    }
    return ret;
}