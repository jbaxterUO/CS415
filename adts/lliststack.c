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
 * implementation for linked-list-based generic stack
 */

#include "ADTs/lliststack.h"
#include <stdlib.h>

typedef struct node {
    struct node *next;
    void *value;
} Node;

typedef struct st_data {
    long count;
    Node *head;
    void (*freeValue)(void *e);
} StData;

/*
 * helper function - traverses stack, applying freeValue function
 * to each element
 */

static void purge(StData *std) {
    Node *p;

    for (p = std->head; p != NULL; p = p->next)
        std->freeValue(p->value);
}

/*
 * helper function to free all nodes in the linked list
 */
static void freeList(StData *std) {
    Node *p, *q = NULL;

    for (p = std->head; p != NULL; p = q) {
        q = p->next;
        free(p);
    }
}

static void st_destroy(const Stack *st) {
    StData *std = (StData *)st->self;

    purge(std);
    freeList(std);
    free(std);                          /* free structure with instance data */
    free((void *)st);                   /* free dispatch table */
}

static void st_clear(const Stack *st) {
    StData *std = (StData *)st->self;

    purge(std);
    freeList(std);
    std->count = 0L;
    std->head = NULL;
}

static bool st_push(const Stack *st, void *element) {
    StData *std = (StData *)st->self;
    Node *p = (Node *)malloc(sizeof(Node));
    bool status = (p != NULL);

    if (status) {
        p->value = element;
        p->next = std->head;
        std->head = p;
        std->count++;
    }
    return status;
}

static bool st_pop(const Stack *st, void **element) {
    StData *std = (StData *)st->self;
    bool status = (std->count > 0L);

    if (status) {
        Node *p = std->head;
        std->head = p->next;
        *element = p->value;
        std->count--;
        free(p);
    }
    return status;
}

static bool st_peek(const Stack *st, void **element) {
    StData *std = (StData *)st->self;
    bool status = (std->count > 0L);

    if (status)
        *element = std->head->value;
    return status;
}

static long st_size(const Stack *st) {
    StData *std = (StData *)st->self;

    return std->count;
}

static bool st_isEmpty(const Stack *st) {
    StData *std = (StData *)st->self;

    return (std->count == 0L);
}

/*
 * helper function - generates array of void * pointers on the heap
 *
 * returns pointer to the array or NULL if malloc failure
 */
static void **genArray(StData *std) {
    void **tmp = NULL;

    if (std->count > 0L) {
        size_t nbytes = std->count * sizeof(void *);
        tmp = (void **)malloc(nbytes);
        if (tmp != NULL) {
            long j = 0L;
            Node *p;

            for (p = std->head; p != NULL; p = p->next)
                tmp[j++] = p->value;
        }
    }
    return tmp;
}

static void **st_toArray(const Stack *st, long *len) {
    StData *std = (StData *)st->self;
    void **tmp = genArray(std);

    if (tmp != NULL)
        *len = std->count;
    return tmp;
}

static const Iterator *st_itCreate(const Stack *st) {
    StData *std = (StData *)st->self;
    const Iterator *it = NULL;
    void **tmp = genArray(std);

    if (tmp != NULL) {
        it = Iterator_create(std->count, tmp);
        if (it == NULL)
            free(tmp);
    }
    return it;
}

static const Stack *st_create(const Stack *st);

static Stack template = {
    NULL, st_create, st_destroy, st_clear, st_push, st_pop, st_peek, st_size,
    st_isEmpty, st_toArray, st_itCreate
};

/*
 * helper function to create a new Stack dispatch table
 */
static const Stack *newStack(void (*freeValue)(void *e)){
    Stack *st = (Stack *)malloc(sizeof(Stack));

    if (st != NULL) {
        StData *std = (StData *)malloc(sizeof(StData));

        if (std != NULL) {
            std->count = 0L;
            std->head = NULL;
            std->freeValue = freeValue;
            *st = template;
            st->self = std;
        } else {
            free(st);
            st = NULL;
        }
    }
    return st;
}
 
static const Stack *st_create(const Stack *st) {
    StData *std = (StData *)st->self;

    return newStack(std->freeValue);
}

const Stack *LListStack(void (*freeValue)(void *e)) {
    return newStack(freeValue);
}

const Stack *Stack_create(void (*freeValue)(void *e)) {
    return newStack(freeValue);
}
