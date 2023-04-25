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
 * implementation for array-based generic stack
 */

#include "ADTs/arraystack.h"
#include <stdlib.h>

typedef struct st_data {
    long capacity;
    long next;
    void **theArray;
    void (*freeValue)(void *e);
} StData;

/*
 * local function - traverses stack, applying user-supplied function
 * to each element
 */

static void purge(StData *std) {
    long i;

    for (i = 0L; i < std->next; i++)
        std->freeValue(std->theArray[i]);       /* free elem storage */
}

static void st_destroy(const Stack *st) {
    StData *std = (StData *)st->self;

    purge(std);                         /* purge remaining elements */
    free(std->theArray);                /* free array of pointers */
    free(std);                          /* free structure with instance data */
    free((void *)st);                   /* free dispatch table */
}

static void st_clear(const Stack *st) {
    StData *std = (StData *)st->self;

    purge(std);
    std->next = 0L;
}

static bool st_push(const Stack *st, void *element) {
    StData *std = (StData *)st->self;
    bool status = (std->next < std->capacity);

    if (! status) {    /* need to reallocate */
        size_t nbytes = 2 * (std->capacity) * sizeof(void *);
        void **tmp = (void **)realloc(std->theArray, nbytes);

        if (tmp != NULL) {
            status = true;
            std->theArray = tmp;
            std->capacity *= 2;
        }
    }
    if (status)
        std->theArray[std->next++] = element;
    return status;
}

static bool st_pop(const Stack *st, void **element) {
    StData *std = (StData *)st->self;
    bool status = (std->next > 0L);

    if (status)
        *element = std->theArray[--std->next];
    return status;
}

static bool st_peek(const Stack *st, void **element) {
    StData *std = (StData *)st->self;
    bool status = (std->next > 0L);

    if (status)
        *element = std->theArray[std->next - 1];
    return status;
}

static long st_size(const Stack *st) {
    StData *std = (StData *)st->self;

    return std->next;
}

static bool st_isEmpty(const Stack *st) {
    StData *std = (StData *)st->self;

    return (std->next == 0L);
}

/*
 * local function - duplicates array of void * pointers on the heap
 *
 * returns pointers to duplicate array or NULL if malloc failure
 */
static void **arrayDupl(StData *std) {
    void **tmp = NULL;

    if (std->next > 0L) {
        size_t nbytes = std->next * sizeof(void *);
        tmp = (void **)malloc(nbytes);
        if (tmp != NULL) {
            long i, j = 0L;

            for (i = std->next - 1; i >= 0L; i--)
                tmp[j++] = std->theArray[i];
        }
    }
    return tmp;
}

static void **st_toArray(const Stack *st, long *len) {
    StData *std = (StData *)st->self;
    void **tmp = arrayDupl(std);

    if (tmp != NULL)
        *len = std->next;
    return tmp;
}

static const Iterator *st_itCreate(const Stack *st) {
    StData *std = (StData *)st->self;
    const Iterator *it = NULL;
    void **tmp = arrayDupl(std);

    if (tmp != NULL) {
        it = Iterator_create(std->next, tmp);
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
static const Stack *newStack(long capacity, void (*freeValue)(void *e)){
    Stack *st = (Stack *)malloc(sizeof(Stack));

    if (st != NULL) {
        StData *std = (StData *)malloc(sizeof(StData));

        if (std != NULL) {
            long cap;
            void **array = NULL;

            cap = (capacity <= 0L) ? DEFAULT_STACK_CAPACITY : capacity;
            array = (void **)malloc(cap * sizeof(void *));
            if (array != NULL) {
                std->capacity = cap;
                std->next = 0L;
                std->theArray = array;
                std->freeValue = freeValue;
                *st = template;
                st->self = std;
            } else {
                free(std);
                free(st);
                st = NULL;
            }
        } else {
            free(st);
            st = NULL;
        }
    }
    return st;
}
 
static const Stack *st_create(const Stack *st) {
    StData *std = (StData *)st->self;

    return newStack(DEFAULT_STACK_CAPACITY, std->freeValue);
}

const Stack *ArrayStack(long capacity, void (*freeValue)(void *e)) {
    return newStack(capacity, freeValue);
}

const Stack *Stack_create(void (*freeValue)(void *e)) {
    return newStack(DEFAULT_STACK_CAPACITY, freeValue);
}
