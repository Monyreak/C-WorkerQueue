// ----------------------------------------------------------------------------
// multiple_threads.c
// Tests multiple threads pushing and popping from a queue
// Author: Mitchell Elliott
// Compile: make
// Run: ./queue-test
// ----------------------------------------------------------------------------

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "queue.h"

#define NUM_ELEMENTS 4
#define NUM_PUSH_THREADS 4
#define NUM_POP_THREADS 4
#define QUEUE_SIZE 4

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;
int ops = 0;
queue_t *q = NULL;

// Thread object struct
struct ThreadObj {
    pthread_t thread;
    size_t tid;
    void *elem;
};

typedef struct ThreadObj *Thread;

// Pushes an element onto the queue
void push_check(queue_t *q, void *elem) {
    bool rtn = queue_push(q, elem);

    // Check if the push was successful
    if (!rtn) {
        fprintf(stderr, "queue_push() failed!\n");
        exit(EXIT_FAILURE);
    }

    return;
}

// Pops an element off the queue
void pop_check(queue_t *q, void *elem) {
    bool rtn = queue_pop(q, elem);

    // Check if the pop was successful
    if (!rtn) {
        fprintf(stderr, "queue_pop() failed!\n");
        exit(EXIT_FAILURE);
    }

    return;
}

// Pusher thread function
void *thread_push(void *arg) {
    Thread threadValue = *(Thread *)arg;

    // Check if the thread args are NULL
    if (threadValue == NULL) {
        fprintf(stderr, "threadValue is NULL!\n");
        exit(EXIT_FAILURE);
    }

    // Check if the queue is NULL
    if (q == NULL) {
        fprintf(stderr, "q is NULL!\n");
        exit(EXIT_FAILURE);
    }

    // ---------------------- Entering Critical Section -----------------------
    pthread_mutex_lock(&mutex);
    push_check(q, (void *)threadValue->elem);
    printf("Pusher thread [%zu] pushed: %lu\n", threadValue->tid, 
        (uintptr_t)threadValue->elem);
    pthread_mutex_unlock(&mutex);
    // ---------------------- Exiting Critical Section ------------------------

    return 0;
}

// Popper thread function
void *thread_pop(void *arg) {
    Thread threadValue = *(Thread *)arg;

    // Check if the thread args are NULL
    if (threadValue == NULL) {
        fprintf(stderr, "threadValue is NULL!\n");
        exit(EXIT_FAILURE);
    }

    // Check if the queue is NULL
    if (q == NULL) {
        fprintf(stderr, "q is NULL!\n");
        exit(EXIT_FAILURE);
    }

    // ---------------------- Entering Critical Section -----------------------
    pthread_mutex_lock(&mutex);
    uintptr_t elem;
    pop_check(q, (void **)&elem);
    printf("Popper thread [%zu] popped: %lu\n", threadValue->tid, elem);
    pthread_mutex_unlock(&mutex);
    // ---------------------- Exiting Critical Section ------------------------

    return 0;
}

int main() {
    q = queue_new(QUEUE_SIZE);

    if (q == NULL) {
        fprintf(stderr, "queue_new() failed!\n");
        exit(EXIT_FAILURE);
    }

    size_t elements[NUM_ELEMENTS];
    Thread pushThreads[NUM_PUSH_THREADS];
    Thread popThreads[NUM_POP_THREADS];
    uintptr_t rc;

    printf("Queue size: %d\n", QUEUE_SIZE);
    printf("Number of pusher threads: %d\n", NUM_PUSH_THREADS);
    printf("Number of popper threads: %d\n", NUM_POP_THREADS);
    printf("Number of elements: %d\n", NUM_ELEMENTS);
    printf("Elements:");
    
    // List of elements to be pushed
    for (size_t i = 0; i < NUM_ELEMENTS; i++) {
        elements[i] = i + 1;
        printf(" %zu", elements[i]);
    }
    
    printf("\n\n");
    printf("-------Pusher Threads-------\n");

    // Create pusher threads
    for (size_t i = 0; i < NUM_PUSH_THREADS; i++) {
        pushThreads[i] = (Thread)malloc(sizeof(struct ThreadObj));
        pushThreads[i]->tid = i + 1;
        pushThreads[i]->elem = (void *)elements[i];
        pthread_create(&pushThreads[i]->thread, NULL, 
            (void *(*)(void*))thread_push, (void *)&pushThreads[i]);
    }

    // Join pusher threads
    for (size_t i = 0; i < NUM_PUSH_THREADS; i++) {
        pthread_join(pushThreads[i]->thread, (void **)&rc);

        if (rc) {
            fprintf(stderr, "pthread_join() failed!\n");
            exit(EXIT_FAILURE);
        }
    }

    printf("----------------------------\n");
    printf("\n");
    printf("-------Popper Threads-------\n");

    // Create popper threads
    for (size_t j = 0; j < NUM_POP_THREADS; j++) {
        popThreads[j] = (Thread)malloc(sizeof(struct ThreadObj));
        popThreads[j]->tid = j + 1;
        pthread_create(&popThreads[j]->thread, NULL, 
            (void *(*)(void*))thread_pop, (void *)&popThreads[j]);
    }

    // Join popper threads
    for (size_t j = 0; j < NUM_POP_THREADS; j++) {
        pthread_join(popThreads[j]->thread, (void **)&rc);

        if (rc) {
            fprintf(stderr, "pthread_join() failed!\n");
            exit(EXIT_FAILURE);
        }
    }

    printf("----------------------------\n");

    // Free memory for pusher threads
    for (size_t i = 0; i < NUM_PUSH_THREADS; i++) {
        if (pushThreads[i] != NULL) {
            free(pushThreads[i]);
        }
    }

    // Free memory for popper threads
    for (size_t j = 0; j < NUM_POP_THREADS; j++) {
        if (popThreads[j] != NULL) {
            free(popThreads[j]);
        }
    }

    // Delete the queue
    queue_delete(&q);

    return 0;
}
