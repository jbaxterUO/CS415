/*
 * Copyright (c) 2018-2019, 2021, University of Oregon
 * All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:

 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * - Neither the name of the University of Oregon nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * implementation for array-based generic FIFO queue
 */

#include "ADTs/arrayqueue.h"
#include <stdlib.h>

typedef struct q_data {
    long count;
    long size;
    int in;
    int out;
    void **buffer;
    void (*freeValue)(void *e);
} QData;

static void purge(QData *qd) {
    int i, n;

    for (i = qd->out, n = qd->count; n > 0; i = (i + 1) % qd->size, n--)
        qd->freeValue(qd->buffer[i]);
}

static void q_destroy(const Queue *q) {
    QData *qd = (QData *)q->self;
    purge(qd);
    free(qd->buffer);
    free(qd);
    free((void *)q);
}

static void q_clear(const Queue *q) {
    QData *qd = (QData *)q->self;

    purge(qd);
    qd->count = 0;
    qd->in = 0;
    qd->out = 0;
}

static bool q_enqueue(const Queue *q, void *element) {
    QData *qd = (QData *)q->self;
    bool status = (qd->count < qd->size);
    
    if (! status) {          /* need to reallocate */
        size_t nbytes = 2 * (qd->size) * sizeof(void *);
        void **tmp = (void **)malloc(nbytes);

        if (tmp != NULL) {
            long n = qd->count, i, j;
            status = true;
            
            for (i = qd->out, j = 0; n > 0; i = (i + 1) % qd->size, j++) {
                tmp[j] = qd->buffer[i];
                n--;
            }
            free(qd->buffer);
            qd->buffer = tmp;
            qd->size *= 2;
            qd->out = 0L;
            qd->in = qd->count;
        }
    }
    if (status) {
        int i = qd->in;
        qd->buffer[i] = element;
        qd->in = (i + 1) % qd->size;
        qd->count++;
    }
    return status;
}

static bool q_front(const Queue *q, void **element) {
    QData *qd = (QData *)q->self;
    bool status = (qd->count > 0L);

    if (status)
        *element = qd->buffer[qd->out];
    return status;
}

static bool q_dequeue(const Queue *q, void **element) {
    QData *qd = (QData *)q->self;
    bool status = (qd->count > 0L);

    if (status) {
        int i = qd->out;
        *element = qd->buffer[i];
        qd->out = (i + 1) % qd->size;
        qd->count--;
    }
    return status;
}

static long q_size(const Queue *q) {
    QData *qd = (QData *)q->self;
    return qd->count;
}

static bool q_isEmpty(const Queue *q) {
    QData *qd = (QData *)q->self;
    return (qd->count == 0L);
}

static void **genArray(QData *qd) {
    void **tmp = NULL;

    if (qd->count > 0L) {
        tmp = (void **)malloc(qd->count * sizeof(void *));
        if (tmp != NULL) {
            int i, j, n;

            n = qd->count;
            for (i = qd->out, j = 0; n > 0; i = (i+1) % qd->size, j++, n--) {
                tmp[j] = qd->buffer[i];
            }
        }
    }
    return tmp;
}

static void **q_toArray(const Queue *q, long *len) {
    QData *qd = (QData *)q->self;
    void **tmp = genArray(qd);

    if (tmp != NULL)
        *len = qd->count;
    return tmp;
}

static const Iterator *q_itCreate(const Queue *q) {
    QData *qd = (QData *)q->self;
    const Iterator *it = NULL;
    void **tmp = genArray(qd);

    if (tmp != NULL) {
        it = Iterator_create(qd->count, tmp);
        if (it == NULL)
            free(tmp);
    }
    return it;
}

static const Queue *q_create(const Queue *q);

static Queue template = {
    NULL, q_create, q_destroy, q_clear, q_enqueue, q_front, q_dequeue, q_size,
    q_isEmpty, q_toArray, q_itCreate
};

/*
 * helper function to create a new Queue dispatch table
 */
static const Queue *newQueue(long capacity, void (*freeValue)(void *e)) {
    Queue *q = (Queue *)malloc(sizeof(Queue));

    if (q != NULL) {
        QData *qd = (QData *)malloc(sizeof(QData));

        if (qd != NULL) {
            long cap;
            void **tmp;

            cap = (capacity <= 0L) ? DEFAULT_QUEUE_CAPACITY : capacity;
            tmp = (void **)malloc(cap * sizeof(void *));
            if (tmp != NULL) {
                qd->count = 0;
                qd->size = cap;
                qd->in = 0;
                qd->out = 0;
                qd->buffer = tmp;
                qd->freeValue = freeValue;
                *q = template;
                q->self = qd;
            } else {
                free(qd);
                free(q);
                q = NULL;
            }
        } else {
            free(q);
            q = NULL;
        }
    }
    return q;
}

static const Queue *q_create(const Queue *q) {
    QData *qd = (QData *)q->self;

    return newQueue(DEFAULT_QUEUE_CAPACITY, qd->freeValue);
}

const Queue *ArrayQueue(long capacity, void (*freeValue)(void *e)) {
    return newQueue(capacity, freeValue);
}

const Queue *Queue_create(void (*freeValue)(void *e)) {
    return newQueue(DEFAULT_QUEUE_CAPACITY, freeValue);
}
