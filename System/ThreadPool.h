//
// Created by z3r0 on 8/22/23.
//

#ifndef SCRAPER_THREADPOOL_H
#define SCRAPER_THREADPOOL_H
#include <stddef.h>
#include <pthread.h>
#include "../DataStructures/Queues/Queue.h"

typedef void (*thread_func_t)(void* args);

typedef struct thread_pool {
    Queue* work_queue;
    pthread_mutex_t work_mutex;
    pthread_cond_t  work_cond;
    pthread_cond_t  working_cond;
    size_t working_cnt;
    size_t thread_cnt;
    int stop;

    void (*add_work)(thread_func_t, void* args, struct thread_pool* pool);
} ThreadPool;

ThreadPool * thread_pool_constructor(int noThreads);
void thread_pool_deconstructor(ThreadPool* pool);
#endif //SCRAPER_THREADPOOL_H
