#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include "queue.h"

typedef struct queue {
    void **data;
    int size;
    int in;
    int out;
    int count;
    pthread_mutex_t lock;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} queue_t;

queue_t *queue_new(int size) {
    queue_t *q = (queue_t *) malloc(sizeof(queue_t));
    if (!q)
        return NULL;

    q->data = (void **) malloc(sizeof(void *) * size);
    if (!q->data) {
        free(q);
        return NULL;
    }

    q->size = size;
    q->in = 0;
    q->out = 0;
    q->count = 0;
    pthread_mutex_init(&q->lock, NULL);
    pthread_cond_init(&q->not_empty, NULL);
    pthread_cond_init(&q->not_full, NULL);

    return q;
}

void queue_delete(queue_t **q) {
    pthread_mutex_destroy(&(*q)->lock);
    pthread_cond_destroy(&(*q)->not_empty);
    pthread_cond_destroy(&(*q)->not_full);

    free((*q)->data);
    free(*q);
    *q = NULL;
}

bool queue_push(queue_t *q, void *elem) {
    if (!q)
        return false;

    pthread_mutex_lock(&q->lock);

    while (q->count == q->size) {
        pthread_cond_wait(&q->not_full, &q->lock);
    }

    q->data[q->in] = elem;
    q->in = (q->in + 1) % q->size;
    q->count++;

    pthread_cond_signal(&q->not_empty);

    pthread_mutex_unlock(&q->lock);

    return true;
}

bool queue_pop(queue_t *q, void **elem) {
    if (!q)
        return false;

    pthread_mutex_lock(&q->lock);

    while (q->count == 0) {
        pthread_cond_wait(&q->not_empty, &q->lock);
    }

    *elem = q->data[q->out];
    q->out = (q->out + 1) % q->size;
    q->count--;

    pthread_cond_signal(&q->not_full);

    pthread_mutex_unlock(&q->lock);

    return true;
}
