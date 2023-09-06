//
// Created by z3r0 on 8/22/23.
//
#include <stdio.h>
#include <stdlib.h>
#include "Queue.h"

void pop_queue (Queue* queue);
void* peek_queue (Queue* queue);
void push_queue (void* data, int dataLen, Queue* queue);

Queue* queue_constructor() {
    Queue* queue = (Queue*)calloc(1, sizeof(Queue));
    if (!queue) {
        fprintf(stderr, "[-] error allocating queue memory\n");
        return NULL;
    }

    queue->queue = linked_list_constructor();
    queue->push = push_queue;
    queue->peek = peek_queue;
    queue->pop = pop_queue;

    return queue;
}

void queue_deconstructor(Queue* queue) {
    linked_list_deconstructor(queue->queue);
}

void pop_queue (Queue* queue) {
    queue->queue->remove(0, queue->queue);
}

void* peek_queue (Queue* queue) {
    return queue->queue->search(0, queue->queue);
}

void push_queue (void* data, int dataLen, Queue* queue) {
    queue->queue->insert(0, data, dataLen, queue->queue);
}