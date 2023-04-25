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
 * implementation for generic array list
 */

#include "ADTs/arraylist.h"
#include <stdlib.h>

typedef struct al_data {
    long capacity;
    long size;
    void **theArray;
    void (*freeValue)(void *e);
} AlData;

/*
 * traverses arraylist, calling freeValue on each element
 */
static void purge(AlData *ald) {
    long i;

    for (i = 0L; i < ald->size; i++) {
        ald->freeValue(ald->theArray[i]);    /* free element storage */
        ald->theArray[i] = NULL;
    }
}

static void al_destroy(const ArrayList *al) {
    AlData *ald = (AlData *)(al->self);
    purge(ald);                           /* purge any remaining elements */
    free(ald->theArray);		  /* free array of pointers */
    free(ald);                            /* free self structures */
    free((void *)al);			  /* free the ArrayList struct */
}

static bool al_add(const ArrayList *al, void *element) {
    AlData *ald = (AlData *)(al->self);
    bool status = (ald->capacity > ald->size);

    if (! status) {	/* need to reallocate */
        size_t nbytes = 2 * ald->capacity * sizeof(void *);
        void **tmp = (void **)realloc(ald->theArray, nbytes);
        if (tmp != NULL) {
            status = true;
            ald->theArray = tmp;
            ald->capacity *= 2;
        }
    }
    if (status)
        ald->theArray[ald->size++] = element;
    return status;
}

static void al_clear(const ArrayList *al) {
    AlData *ald = (AlData *)(al->self);
    purge(ald);
    ald->size = 0L;
}

static bool al_ensureCapacity(const ArrayList *al, long minCapacity) {
    AlData *ald = (AlData *)(al->self);
    int status = true;

    if (ald->capacity < minCapacity) {	/* must extend */
        void **tmp = (void **)realloc(ald->theArray,
                                      minCapacity * sizeof(void *));
        if (tmp == NULL)
            status = false;	/* allocation failure */
        else {
            ald->theArray = tmp;
            ald->capacity = minCapacity;
        }
    }
    return status;
}

static bool al_get(const ArrayList *al, long index, void **element) {
    AlData *ald = (AlData *)(al->self);
    bool status = (index >= 0L && index < ald->size);

    if (status)
        *element = ald->theArray[index];
    return status;
}

static bool al_insert(const ArrayList *al, long index, void *element) {
    AlData *ald = (AlData *)(al->self);
    bool status = (ald->size < ald->capacity);

    if (index > ald->size)
        return false;				/* 0 <= index <= size */
    if (! status) {	/* need to reallocate */
        size_t nbytes = 2 * ald->capacity * sizeof(void *);
        void **tmp = (void **)realloc(ald->theArray, nbytes);
        if (tmp != NULL) {
            ald->theArray = tmp;
            ald->capacity *= 2;
            status = true;
        }
    }
    if (status) {
        long j;
        for (j = ald->size; j > index; j--)		/* slide items up */
            ald->theArray[j] = ald->theArray[j-1];
        ald->theArray[index] = element;
        ald->size++;
    }
    return status;
}

static bool al_isEmpty(const ArrayList *al) {
    AlData *ald = (AlData *)(al->self);
    return (ald->size == 0L);
}

static bool al_remove(const ArrayList *al, long index) {
    AlData *ald = (AlData *)(al->self);
    bool status = (index >= 0L && index < ald->size);
    long j;

    if (status) {
        void *element = ald->theArray[index];
        for (j = index + 1; j < ald->size; j++)
            ald->theArray[index++] = ald->theArray[j];
        ald->size--;
        ald->freeValue(element);
    }
    return status;
}

static bool al_set(const ArrayList *al, long index, void *element) {
    AlData *ald = (AlData *)(al->self);
    bool status = (index >= 0L && index < ald->size);

    if (status) {
        void *previous = ald->theArray[index];
        ald->theArray[index] = element;
        ald->freeValue(previous);
    }
    return status;
}

static long al_size(const ArrayList *al) {
    AlData *ald = (AlData *)(al->self);
    return ald->size;
}

/*
 * local function that duplicates the array of void * pointers on the heap
 *
 * returns pointer to duplicate array or NULL if malloc failure
 */
static void **arraydupl(AlData *ald) {
    void **tmp = NULL;
    if (ald->size > 0L) {
        size_t nbytes = ald->size * sizeof(void *);
        tmp = (void **)malloc(nbytes);
        if (tmp != NULL) {
            long i;

            for (i = 0; i < ald->size; i++)
                tmp[i] = ald->theArray[i];
        }
    }
    return tmp;
}

static void **al_toArray(const ArrayList *al, long *len) {
    AlData *ald = (AlData *)al->self;
    void **tmp = arraydupl(ald);

    if (tmp != NULL)
        *len = ald->size;
    return tmp;
}

static bool al_trimToSize(const ArrayList *al) {
    AlData *ald = (AlData *)al->self;
    int status = false;

    void **tmp = (void **)realloc(ald->theArray, ald->size * sizeof(void *));
    if (tmp != NULL) {
        status = true;
        ald->theArray = tmp;
        ald->capacity = ald->size;
    }
    return status;
}

static const Iterator *al_itCreate(const ArrayList *al) {
    AlData *ald = (AlData *)al->self;
    const Iterator *it = NULL;
    void **tmp = arraydupl(ald);

    if (tmp != NULL) {
        it = Iterator_create(ald->size, tmp);
        if (it == NULL)
            free(tmp);
    }
    return it;
}

static ArrayList template = {
    NULL, al_destroy, al_add, al_clear, al_ensureCapacity, al_get, al_insert,
    al_isEmpty, al_remove, al_set, al_size, al_toArray, al_trimToSize,
    al_itCreate
};

const ArrayList *ArrayList_create(long capacity, void (*freeValue)(void *e)) {
    ArrayList *al = (ArrayList *)malloc(sizeof(ArrayList));

    if (al != NULL) {
        AlData *ald = (AlData *)malloc(sizeof(AlData));
        if (ald != NULL) {
            long cap = (capacity <= 0) ? DEFAULT_ARRAYLIST_CAPACITY : capacity;
            void **array = (void **) malloc(cap * sizeof(void *));

            if (array != NULL) {
                ald->capacity = cap;
                ald->size = 0L;
                ald->theArray = array;
                ald->freeValue = freeValue;
                *al = template;
                al->self = ald;
            } else {
                free(ald);
                free(al);
                al = NULL;
            }
        } else {
            free(al);
            al = NULL;
        }
    }
    return al;
}
