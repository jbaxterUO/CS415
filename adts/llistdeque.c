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
 * implementation for generic deque using a linked list with a sentinel
 */

#include "ADTs/llistdeque.h"
#include <stdlib.h>

#define SENTINEL(p) (&(p)->sentinel)

typedef struct llnode {
    struct llnode *next;
    struct llnode *prev;
    void *element;
} LLNode;

typedef struct d_data {
    long size;
    LLNode sentinel;
    void (*freeValue)(void *e);
} DData;

/*
 * link `p' between `before' and `after'
 * must work correctly if `before' and `after' are the same node
 * (i.e. the sentinel)
 */
static void link(LLNode *before, LLNode *p, LLNode *after) {
    p->next = after;
    p->prev = before;
    after->prev = p;
    before->next = p;
}

/*
 * unlinks the LLNode from the doubly-linked list
 */
static void unlink(LLNode *p) {
    p->prev->next = p->next;
    p->next->prev = p->prev;
}

/*
 * traverses linked list, calling freeValue on each element
 */
static void purge(DData *dd) {
    LLNode *p;

    for (p = dd->sentinel.next; p != SENTINEL(dd); p = p->next)
        dd->freeValue(p->element);
}

/*
 * frees the nodes from the doubly-linked list
 */
static void freeList(DData *dd) {
    LLNode *p = dd->sentinel.next;

    while (p != SENTINEL(dd)) {
        LLNode *q = p->next;
        free(p);
        p = q;
    }
}

static void d_destroy(const Deque *d) {
    DData *dd = (DData *)d->self;
    purge(dd);
    freeList(dd);
    free(dd);
    free((void *)d);
}

static void d_clear(const Deque *d) {
    DData *dd = (DData *)d->self;
    purge(dd);
    freeList(dd);
    dd->size = 0L;
    dd->sentinel.next = SENTINEL(dd);
    dd->sentinel.prev = SENTINEL(dd);
}

static bool d_insertFirst(const Deque *d, void *element) {
    DData *dd = (DData *)d->self;
    LLNode *p = (LLNode *)malloc(sizeof(LLNode));
    bool status = (p != NULL);

    if (status) {
        p->element = element;
        link(SENTINEL(dd), p, SENTINEL(dd)->next);
        dd->size++;
    }
    return status;
}

static bool d_insertLast(const Deque *d, void *element) {
    DData *dd = (DData *)d->self;
    LLNode *p = (LLNode *)malloc(sizeof(LLNode));
    bool status = (p != NULL);

    if (status) {
        p->element = element;
        link(SENTINEL(dd)->prev, p, SENTINEL(dd));
        dd->size++;
    }
    return status;
}

static bool d_first(const Deque *d, void **element) {
    DData *dd = (DData *)d->self;
    LLNode *p = SENTINEL(dd)->next;
    bool status = (p != SENTINEL(dd));

    if (status)
        *element = p->element;
    return status;
}

static bool d_last(const Deque *d, void **element) {
    DData *dd = (DData *)d->self;
    LLNode *p = SENTINEL(dd)->prev;
    bool status = (p != SENTINEL(dd));

    if (status)
        *element = p->element;
    return status;
}

static bool d_removeFirst(const Deque *d, void **element) {
    DData *dd = (DData *)d->self;
    LLNode *p = SENTINEL(dd)->next;
    bool status = (p != SENTINEL(dd));

    if (status) {
        *element = p->element;
        unlink(p);
        free(p);
        dd->size--;
    }
    return status;
}

static bool d_removeLast(const Deque *d, void **element) {
    DData *dd = (DData *)d->self;
    LLNode *p = SENTINEL(dd)->prev;
    bool status = (p != SENTINEL(dd));

    if (status) {
        *element = p->element;
        unlink(p);
        free(p);
        dd->size--;
    }
    return status;
}

static long d_size(const Deque *d) {
    DData *dd = (DData *)d->self;
    return dd->size;
}

static bool d_isEmpty(const Deque *d) {
    DData *dd = (DData *)d->self;
    return (dd->size == 0L);
}

/*
 * helper function to generate array of element values on the heap
 *
 * returns pointer to array or NULL if malloc failure
 */
static void **genArray(DData *dd) {
    void **tmp = NULL;
    if (dd->size > 0L) {
        size_t nbytes = dd->size * sizeof(void *);
        tmp = (void **)malloc(nbytes);
        if (tmp != NULL) {
            long i; 
            LLNode *p;
            for (i = 0, p = SENTINEL(dd)->next; i < dd->size; i++, p = p->next)
                tmp[i] = p->element;
        }
    }
    return tmp;
}

static void **d_toArray(const Deque *d, long *len) {
    DData *dd = (DData *)d->self;
    void **tmp = genArray(dd);

    if (tmp != NULL)
        *len = dd->size;
    return tmp;
}

static const Iterator *d_itCreate(const Deque *d) {
    DData *dd = (DData *)d->self;
    const Iterator *it = NULL;
    void **tmp = genArray(dd);

    if (tmp != NULL) {
        it = Iterator_create(dd->size, tmp);
        if (it == NULL)
            free(tmp);
    }
    return it;
}

static const Deque *d_create(const Deque *d);

static Deque template = {
    NULL, d_create, d_destroy, d_clear, d_insertFirst, d_insertLast, d_first,
    d_last, d_removeFirst, d_removeLast, d_size, d_isEmpty, d_toArray,
    d_itCreate
};

/*
 * helper function to create a new Deque dispatch table
 */
static const Deque *newDeque(void (*freeValue)(void *e)) {
    Deque *d = (Deque *)malloc(sizeof(Deque));

    if (d != NULL) {
        DData *dd = (DData *)malloc(sizeof(DData));
        if (dd != NULL) {
            dd->size = 0L;
            dd->sentinel.next = SENTINEL(dd);
            dd->sentinel.prev = SENTINEL(dd);
            dd->freeValue = freeValue;
            *d = template;
            d->self = dd;
        } else {
            free(d);
            d = NULL;
        }
    }
    return d;
}

static const Deque *d_create(const Deque *d) {
    DData *dd = (DData *)d->self;

    return newDeque(dd->freeValue);
}

const Deque *LListDeque(void (*freeValue)(void *e)) {
    return newDeque(freeValue);
}

const Deque *Deque_create(void (*freeValue)(void *e)) {
    return newDeque(freeValue);
}
