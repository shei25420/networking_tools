//
// Created by shei on 9/4/23.
//

#ifndef SCRAPER_STACK_H
#define SCRAPER_STACK_H
typedef struct stack {
    void (*push)(void* data, struct stack* stack);
    void* (*pop)(struct stack* stack);
    int total;
} Stack;



#endif //SCRAPER_STACK_H
