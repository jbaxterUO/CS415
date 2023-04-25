/*
 * Copyright (c) 2019, 2021, University of Oregon
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
 * implementation for linked-list-based generic FIFO queue
 */

#include "ADTs/llistqueue.h"
#include <stdlib.h>

typedef struct node {
    struct node *next;
    void *value;
} Node;

typedef struct q_data {
    long count;
    Node *head;
    Node *tail;
    void (*freeValue)(void *e);
} QData;

static void purge(QData *qd) {
    Node *p;

    for (p = qd->head; p != NULL; p = p->next)
        qd->freeValue(p->value);
}

static void freeList(QData *qd) {
    Node *p, *q = NULL;

    for (p = qd->head; p != NULL; p = q) {
        q = p->next;
        free(p);
    }
}

static void q_destroy(const Queue *q) {
    QData *qd = (QData *)q->self;
    purge(qd);
    freeList(qd);
    free(qd);
    free((void *)q);
}

static void q_clear(const Queue *q) {
    QData *qd = (QData *)q->self;

    purge(qd);
    freeList(qd);
    qd->count = 0;
    qd->head = NULL;
    qd->tail = NULL;
}

static bool q_enqueue(const Queue *q, void *element) {
    QData *qd = (QData *)q->self;
    Node *p = (Node *)malloc(sizeof(Node));
    bool status = (p != NULL);
    
    if (status) {
        p->next = NULL;
        p->value = element;
        if (qd->tail == NULL)
            qd->head = p;
        else
            qd->tail->next = p;
        qd->tail = p;
        qd->count++;
    }
    return status;
}

static bool q_front(const Queue *q, void **element) {
    QData *qd = (QData *)q->self;
    bool status = (qd->count > 0L);

    if (status)
        *element = qd->head->value;
    return status;
}

static bool q_dequeue(const Queue *q, void **element) {
    QData *qd = (QData *)q->self;
    bool status = (qd->count > 0L);

    if (status) {
        Node *p = qd->head;
        if ((qd->head = p->next) == NULL)
            qd->tail = NULL;
        qd->count--;
        *element = p->value;
        free(p);
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
            long n = 0L;
            Node *p;

            for (p = qd->head; p != NULL; p = p->next)
                tmp[n++] = p->value;
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
static const Queue *newQueue(void (*freeValue)(void *e)) {
    Queue *q = (Queue *)malloc(sizeof(Queue));

    if (q != NULL) {
        QData *qd = (QData *)malloc(sizeof(QData));

        if (qd != NULL) {
            qd->count = 0;
            qd->head = NULL;
            qd->tail = NULL;
            qd->freeValue = freeValue;
            *q = template;
            q->self = qd;
        } else {
            free(q);
            q = NULL;
        }
    }
    return q;
}

static const Queue *q_create(const Queue *q) {
    QData *qd = (QData *)q->self;

    return newQueue(qd->freeValue);
}

const Queue *LListQueue(void (*freeValue)(void *e)) {
    return newQueue(freeValue);
}

const Queue *Queue_create(void (*freeValue)(void *e)) {
    return newQueue(freeValue);
}
