//
// Created by z3r0 on 8/22/23.
//

#ifndef SCRAPER_QUEUE_H
#define SCRAPER_QUEUE_H
#include "../LIsts/LinkedLists.h"

typedef struct queue {
    LinkedList* queue;
    void (*push)(void* data, int dataLen, struct queue* queue);
    void* (*peek)(struct queue* queue);
    void (*pop)(struct queue* queue);
} Queue;

Queue* queue_constructor();
void queue_deconstructor(Queue* queue);
#endif //SCRAPER_QUEUE_H
