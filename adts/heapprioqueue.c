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
 * implementation for generic priority queue, for generic priorities
 * implemented using a heap that expands when needed
 */

#include "ADTs/heapprioqueue.h"
#include <stdlib.h>

#define DEFAULT_HEAP_SIZE 25

typedef struct pqentry {
    void *priority;
    void *value;
    long sequenceNo;
} PQEntry;

typedef struct pq_data {
    int (*cmp)(void *p1, void *p2);
    long sequenceNo;
    long last;
    long size;
    PQEntry *heap;
    void (*freePrio)(void *p);
    void (*freeValue)(void *v);
} PqData;

/*
 * helper function to perform comparisons
 *
 * in order to guarantee FIFO, we first compare priorities using cmp() - if
 * that yields 0, then we return the difference in sequenceNo values
 */
static int realCmp(PqData *pqd, PQEntry *p1, PQEntry *p2) {
    int ans;
    if ((ans = pqd->cmp(p1->priority, p2->priority)) == 0)
        ans =  (int)(p1->sequenceNo - p2->sequenceNo);
    return ans;
}

/*
 * traverses the heap, calling freeP on each priority and freeV on each entry
 */
static void purge(PqData *pqd) {
    long i;

    for (i = 1; i <= pqd->last; i++) {
        pqd->freePrio(pqd->heap[i].priority);
        pqd->freeValue(pqd->heap[i].value);
    }
}

static void pq_destroy(const PrioQueue *pq) {
    PqData *pqd = (PqData *)pq->self;
    purge(pqd);
    free(pqd->heap);
    free(pqd);
    free((void *)pq);
}

static void pq_clear(const PrioQueue *pq) {
    PqData *pqd = (PqData *)pq->self;
    purge(pqd);
    pqd->last = 0L;
}

/*
 *  the siftup function restores the heap property after adding a new entry
 *  preconditions: last > 0 && heap(1,last-1)
 *  postcondition: heap(1,last)
 */
static void siftup(PqData *pqd) {
    long p, i = pqd->last;

    while (i > 1) {
        PQEntry hn;
        p = i / 2;
        if (realCmp(pqd, &(pqd->heap[p]), &(pqd->heap[i])) <= 0)
            break;
        hn = pqd->heap[p];
        pqd->heap[p] = pqd->heap[i];
        pqd->heap[i] = hn;
        i = p;
    }
}

static bool pq_insert(const PrioQueue *pq, void *priority, void *value) {
    PqData *pqd = (PqData *)pq->self;
    long i = pqd->last + 1;
    bool status = (i < pqd->size);
    
    if (! status) {       /* need to resize the array */
        size_t nbytes = (2 * pqd->size) * sizeof(PQEntry);
        PQEntry *tmp = (PQEntry *)realloc(pqd->heap, nbytes);

        if (tmp != NULL) {
            status = true;
            pqd->heap = tmp;
            pqd->size *= 2;
        }
    }
    if (status) {
        pqd->heap[i].priority = priority;
        pqd->heap[i].value = value;
        pqd->heap[i].sequenceNo = pqd->sequenceNo++;
        pqd->last = i;
        siftup(pqd);
    }
    return status;
}

static bool pq_min(const PrioQueue *pq, void **priority, void **value) {
    PqData *pqd = (PqData *)pq->self;
    bool status = (pqd->last > 0L);

    if (status) {
        *priority = (pqd->heap[1].priority);
        *value = (pqd->heap[1].value);
    }
    return status;
}

/*
 *  the siftdown function restores the heap property after removing
 *  the top element, and replacing it by the previous last element
 *  preconditions: heap(2,last) && last >= 0
 *  postcondition: heap(1,last-1)
 */
static void siftdown(PqData *pqd) {
    long c, i;

    i = 1;
    for(;;) {
        PQEntry hn;
        c = 2 * i;
        if (c > pqd->last)
            break;
        if ((c+1) <= pqd->last &&
            realCmp(pqd, &(pqd->heap[c+1]), &(pqd->heap[c])) < 0)
            c++;
        if (realCmp(pqd, &(pqd->heap[i]), &(pqd->heap[c])) <= 0)
            break;
        hn = pqd->heap[i];
        pqd->heap[i] = pqd->heap[c];
        pqd->heap[c] = hn;
        i = c;
    }
}

static bool pq_removeMin(const PrioQueue *pq, void **priority, void **value) {
    PqData *pqd = (PqData *)pq->self;
    bool status = (pqd->last > 0L);

    if (status) {
        *priority = (pqd->heap[1].priority);
        *value = (pqd->heap[1].value);
        pqd->heap[1] = pqd->heap[pqd->last];
        pqd->last--;
        siftdown(pqd);
    }
    return status;
}

static long pq_size(const PrioQueue *pq) {
    PqData *pqd = (PqData *)pq->self;
    return pqd->last;
}

static bool pq_isEmpty(const PrioQueue *pq) {
    PqData *pqd = (PqData *)pq->self;
    return (pqd->last == 0L);
}

/*
 * helper function to generate array of void *'s for toArray and itCreate
 */
static void **genArray(PqData *pqd) {
    void **theArray = NULL;
    if (pqd->last >0L) {
        PqData npqd = *pqd;
        PQEntry *tmp = (PQEntry *)malloc((pqd->last+1)*sizeof(PQEntry));
        if (tmp != NULL) {
            long i;
            theArray = (void **)malloc(pqd->last*sizeof(void *));
            if (theArray != NULL) {
                for (i = 0; i < pqd->last + 1; i++)  /* copy the heap */
                    tmp[i] = pqd->heap[i];
                npqd.heap = tmp;
                /* copy min element into theArray, swap first with last
                   and siftdown */
                for (i = 0; i < pqd->last; i++) {
                    theArray[i] = tmp[1].value;
                    tmp[1] = tmp[npqd.last--];
                    siftdown(&npqd);
                }
            }
            free(tmp);
        }
    }
    return theArray;
}

static void **pq_toArray(const PrioQueue *pq, long *len) {
    PqData *pqd = (PqData *)pq->self;
    void **tmp = genArray(pqd);
    if (tmp != NULL)
        *len = pqd->last;
    return tmp;
}

static const Iterator *pq_itCreate(const PrioQueue *pq) {
    PqData *pqd =(PqData *)pq->self;
    const Iterator *it = NULL;
    void **tmp = genArray(pqd);
    if (tmp != NULL) {
        it = Iterator_create(pqd->last, tmp);
        if (it == NULL)
            free(tmp);
    }
    return it;
}

static const PrioQueue *pq_create(const PrioQueue *pq);

static PrioQueue template = {
    NULL, pq_create, pq_destroy, pq_clear, pq_insert, pq_min, pq_removeMin,
    pq_size, pq_isEmpty, pq_toArray, pq_itCreate
};

/*
 * helper function to create a new Priority Queue dispatch table
 */
static const PrioQueue *newPrioQueue(int (*cmp)(void*, void*),
                                     void (*freeP)(void*),
                                     void (*freeV)(void*)) {
    PrioQueue *pq = (PrioQueue *)malloc(sizeof(PrioQueue));

    if (pq != NULL) {
        PqData *pqd = (PqData *)malloc(sizeof(PqData));

        if (pqd != NULL) {
            PQEntry *p = (PQEntry *)malloc(DEFAULT_HEAP_SIZE * sizeof(PQEntry));

            if (p != NULL) {
                pqd->cmp = cmp;
                pqd->sequenceNo = 0L;
                pqd->size = DEFAULT_HEAP_SIZE;
                pqd->last = 0L;
                pqd->heap = p;
                pqd->freePrio = freeP;
                pqd->freeValue = freeV;
                *pq = template;
                pq->self = pqd;
            } else {
                free(pqd);
                free(pq);
                pq = NULL;
            }
        } else {
            free(pq);
            pq = NULL;
        }
    }
    return pq;
}

static const PrioQueue *pq_create(const PrioQueue *pq) {
    PqData *pqd = (PqData *)pq->self;

    return newPrioQueue(pqd->cmp, pqd->freePrio, pqd->freeValue);
}

const PrioQueue *HeapPrioQueue(int (*cmp)(void *p1, void *p2),
                               void (*freePrio)(void *prio),
                               void (*freeValue)(void *value)) {
    return newPrioQueue(cmp, freePrio, freeValue);
}

const PrioQueue *PrioQueue_create(int (*cmp)(void *p1, void *p2),
                                  void (*freePrio)(void *prio),
                                  void (*freeValue)(void *value)) {
    return newPrioQueue(cmp, freePrio, freeValue);
}
