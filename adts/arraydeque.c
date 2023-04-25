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
 * implementation for generic deque using an array
 */

#include "ADTs/arraydeque.h"
#include <stdlib.h>

typedef struct d_data {
    long size;
    long count;
    long head;
    long tail;
    void **buffer;
    void (*freeValue)(void *e);
} DData;

/*
 * traverses deque, calling freeValue on each element
 */
static void purge(DData *dd) {
    long i, n;

    for (i = dd->head, n = dd->count; n > 0L; n--) {
        dd->freeValue(dd->buffer[i]);
        i = (i + 1) % dd->size;
    }
}

static void d_destroy(const Deque *d) {
    DData *dd = (DData *)d->self;
    purge(dd);
    free(dd->buffer);
    free(dd);
    free((void *)d);
}

static void d_clear(const Deque *d) {
    DData *dd = (DData *)d->self;
    purge(dd);
    dd->count = 0L;
    dd->head = dd->tail = 0L;
}

/*
 * helper function to resize array
 *
 * assumes that array is full
 */
static bool resizeArray(DData *dd) {
    long nbytes = 2 * dd->size * sizeof(void *);
    void **tmp = (void **)malloc(nbytes);
    int status = (tmp != NULL);
    if (status) {
        long n = dd->count, i, j;
        for (i = dd->head, j = 0; n > 0L; i = (i + 1) % dd->size, j++) {
            tmp[j] = dd->buffer[i];
            n--;
        }
        free(dd->buffer);
        dd->buffer = tmp;
        dd->size *= 2;
        dd->head = 0L;
        dd->tail = dd->count - 1;
    }
    return status;
}

static bool d_insertFirst(const Deque *d, void *element) {
    DData *dd = (DData *)d->self;
    bool status = (dd->count < dd->size);

    if (! status)  /* array is full, resize */
        status = resizeArray(dd);
    if (status) {
        if (dd->count == 0L) {
            dd->head = 0L;
            dd->tail = 0L;
            dd->buffer[0L] = element;
        } else {
            long i = (dd->head == 0L) ? dd->size - 1 : dd->head - 1;
            dd->buffer[i] = element;
            dd->head = i;
        }
        dd->count++;
    }
    return status;
}

static bool d_insertLast(const Deque *d, void *element) {
    DData *dd = (DData *)d->self;
    bool status = (dd->count < dd->size);

    if (! status)  /* array is full, resize */
        status = resizeArray(dd);
    if (status) {
        if (dd->count == 0L) {
            dd->head = 0L;
            dd->tail = 0L;
            dd->buffer[0L] = element;
        } else {
            long i = ((dd->tail + 1) == dd->size) ? 0L : dd->tail + 1;
            dd->buffer[i] = element;
            dd->tail = i;
        }
        dd->count++;
    }
    return status;
}

static bool d_first(const Deque *d, void **element) {
    DData *dd = (DData *)d->self;
    bool status = (dd->count > 0L);

    if (status)
        *element = dd->buffer[dd->head];
    return status;
}

static bool d_last(const Deque *d, void **element) {
    DData *dd = (DData *)d->self;
    bool status = (dd->count > 0L);

    if (status)
        *element = dd->buffer[dd->tail];
    return status;
}

static bool d_removeFirst(const Deque *d, void **element) {
    DData *dd = (DData *)d->self;
    bool status = (dd->count > 0L);

    if (status) {
        long i = dd->head;
        *element = dd->buffer[i];
        dd->head = (i + 1) % dd->size;
        dd->count--;
    }
    return status;
}

static bool d_removeLast(const Deque *d, void **element) {
    DData *dd = (DData *)d->self;
    bool status = (dd->count > 0L);

    if (status) {
        long i = dd->tail;
        *element = dd->buffer[i];
        dd->tail = (i == 0L) ? dd->size - 1 : --i;
        dd->count--;
    }
    return status;
}

static long d_size(const Deque *d) {
    DData *dd = (DData *)d->self;
    return dd->count;
}

static bool d_isEmpty(const Deque *d) {
    DData *dd = (DData *)d->self;
    return (dd->count == 0L);
}

/*
 * helper function to generate array of element values on the heap
 *
 * returns pointer to array or NULL if malloc failure
 */
static void **genArray(DData *dd) {
    void **tmp = NULL;
    if (dd->count > 0L) {
        size_t nbytes = dd->count * sizeof(void *);
        tmp = (void **)malloc(nbytes);
        if (tmp != NULL) {
            long i, j;
            for (i = 0, j = dd->head; i < dd->count; i++, j = (j + 1) % dd->size)
                tmp[i] = dd->buffer[j];
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
static const Deque *newDeque(long capacity, void (*freeValue)(void *e)) {
    Deque *d = (Deque *)malloc(sizeof(Deque));

    if (d != NULL) {
        DData *dd = (DData *)malloc(sizeof(DData));
        if (dd != NULL) {
            long cap = (capacity > 0L) ? capacity : DEFAULT_DEQUE_CAPACITY;
            void **tmp = (void **)malloc(cap * sizeof(void *));
            if (tmp != NULL) {
                dd->size = cap;
                dd->count = 0L;
                dd->head = dd->tail = 0L;
                dd->buffer = tmp;
                dd->freeValue = freeValue;
                *d = template;
                d->self = dd;
             } else {
                free(dd);
                free(d);
                d = NULL;
             }
        } else {
            free(d);
            d = NULL;
        }
    }
    return d;
}

static const Deque *d_create(const Deque *d) {
    DData *dd = (DData *)d->self;

    return newDeque(DEFAULT_DEQUE_CAPACITY, dd->freeValue);
}

const Deque *ArrayDeque(long capacity, void (*freeValue)(void *e)) {
    return newDeque(capacity, freeValue);
}

const Deque *Deque_create(void (*freeValue)(void *e)) {
    return newDeque(DEFAULT_DEQUE_CAPACITY, freeValue);
}
