//
// Created by z3r0 on 8/22/23.
//
#include <stdio.h>
#include <stdlib.h>
#include <sys/sysinfo.h>
#include "ThreadPool.h"

typedef struct thread_worker {
    thread_func_t func;
    void* args;
} ThreadWorker;

void* thread_pool_worker (void* args);
void thread_pool_wait (ThreadPool* pool);
void add_thread_worker (thread_func_t, void* args, ThreadPool* pool);

ThreadPool * thread_pool_constructor(int noThreads) {
    pthread_t thread_id;

    ThreadPool * pool = (ThreadPool*)calloc(1, sizeof(ThreadPool));
    if (!pool) {
        fprintf(stderr, "[-] error allocating thread pool memory\n");
        return NULL;
    }

    if (!noThreads) noThreads = get_nprocs() + 1;

    pool->work_queue = queue_constructor();
    if (!(pool->work_queue)) {
        free(pool);
        return NULL;
    }

    pthread_mutex_init(&(pool->work_mutex), NULL);
    pthread_cond_init(&(pool->work_cond), NULL);
    pthread_cond_init(&(pool->working_cond), NULL);

    pool->thread_cnt = noThreads;
    pool->add_work = add_thread_worker;

    for (int i = 0; i < pool->thread_cnt; i++) {
        pthread_create(&thread_id, NULL, thread_pool_worker, pool);
        pthread_detach(thread_id);
    }
    return pool;
}

void thread_pool_deconstructor(ThreadPool* pool) {
    ThreadWorker *work;
    ThreadWorker *work2;

    if (!pool || !pool->work_queue) return;

    pthread_mutex_lock(&(pool->work_mutex));
    work = pool->work_queue->peek(pool->work_queue);
    while (work != NULL) {
        pool->work_queue->pop(pool->work_queue);
        work = pool->work_queue->peek(pool->work_queue);
    }

    pool->stop = 1;
    pthread_cond_broadcast(&(pool->work_cond));
    pthread_mutex_unlock(&(pool->work_mutex));

    thread_pool_wait(pool);

    pthread_mutex_destroy(&(pool->work_mutex));
    pthread_cond_destroy(&(pool->work_cond));
    pthread_cond_destroy(&(pool->working_cond));

    free(pool);
}

void add_thread_worker (thread_func_t func, void* args, ThreadPool* pool) {
    ThreadWorker worker = { 0 };
    worker.func = func;
    worker.args = args;

    pthread_mutex_lock(&(pool->work_mutex));
    pool->work_queue->push(&worker, sizeof(worker), pool->work_queue);

    pthread_cond_broadcast(&(pool->work_cond));
    pthread_mutex_unlock(&(pool->work_mutex));
}

void* thread_pool_worker (void* args) {
    ThreadPool * pool = (ThreadPool*)args;
    while (1) {
        pthread_mutex_lock(&(pool->work_mutex));
        while (!(pool->work_queue->queue->head) && !(pool->stop)) {
            pthread_cond_wait(&(pool->work_cond), &(pool->work_mutex));
        }

        if (pool->stop) break;

        ThreadWorker * work = pool->work_queue->peek(pool->work_queue);
        pool->working_cnt++;

        pthread_mutex_unlock(&(pool->work_mutex));
        if (work != NULL) {
            work->func(work->args);
        }

        pthread_mutex_lock(&(pool->work_mutex));
        pool->working_cnt--;

        if (!pool->stop && (pool->working_cnt == 0) && (pool->work_queue->queue->head)) {
            pthread_cond_signal(&(pool->working_cond));
        }
        pthread_mutex_unlock(&(pool->work_mutex));
    }

    pool->thread_cnt--;
    pthread_cond_signal(&(pool->working_cond));
    pthread_mutex_unlock(&(pool->work_mutex));

    return NULL;
}

void thread_pool_wait (ThreadPool* pool) {
    if (pool == NULL) return;

    pthread_mutex_lock(&(pool->work_mutex));
    while (1) {
        if ((!(pool->stop) && (pool->working_cnt != 0)) || (!(pool->stop) && (pool->thread_cnt != 0))) {
            pthread_cond_wait(&(pool->working_cond), &(pool->work_mutex));
        } else break;
    }
    pthread_mutex_unlock(&(pool->work_mutex));
}