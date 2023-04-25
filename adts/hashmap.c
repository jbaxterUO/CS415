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
 * implementation for generic hashmap
 */

#include "ADTs/hashmap.h"
#include <stdlib.h>

#define DEFAULT_CAPACITY 16
#define MAX_CAPACITY 134217728L
#define DEFAULT_LOAD_FACTOR 0.75
#define TRIGGER 100	/* number of changes that will trigger a load check */

typedef struct node {
    struct node *next;
    MEntry entry;
} Node;

typedef struct m_data {
    long (*hash)(void *, long N);
    int (*cmp)(void *, void *);
    long size;
    long capacity;
    long changes;
    double load;
    double loadFactor;
    double increment;
    Node **buckets;
    void (*freeK)(void *k);
    void (*freeV)(void *v);
} MData;

/*
 * traverses the map, calling freeK and freeV on each entry
 * then frees storage associated with the MEntry structure
 */
static void purge(MData *md) {
    long i;

    for (i = 0L; i < md->capacity; i++) {
        Node *p, *q;
        p = md->buckets[i];
        while (p != NULL) {
            md->freeK((p->entry).key);
            md->freeV((p->entry).value);
            q = p->next;
            free(p);
            p = q;
        }
        md->buckets[i] = NULL;
    }
}

static void m_destroy(const Map *m) {
    MData *md = (MData *)m->self;
    purge(md);
    free(md->buckets);
    free(md);
    free((void *)m);
}

static void m_clear(const Map *m) {
    MData *md = (MData *)m->self;
    purge(md);
    md->size = 0;
    md->load = 0.0;
    md->changes = 0;
}

/*
 * local function to locate key in a map
 *
 * returns pointer to entry, if found, as function value; NULL if not found
 * returns bucket index in `bucket'
 */
static Node *findKey(MData *md, void *key, long *bucket) {
    long i = md->hash(key, md->capacity);
    Node *p;

    *bucket = i;
    for (p = md->buckets[i]; p != NULL; p = p->next) {
        if (md->cmp((p->entry).key, key) == 0) {
            break;
        }
    }
    return p;
}

static bool m_containsKey(const Map *m, void *key) {
    MData *md = (MData *)m->self;
    long bucket;

    return (findKey(md, key, &bucket) != NULL);
}

static bool m_get(const Map *m, void *key, void **value) {
    MData *md = (MData *)m->self;
    long i;
    Node *p = findKey(md, key, &i);
    bool status = (p != NULL);

    if (status)
        *value = (p->entry).value;
    return status;
}

/*
 * helper function that resizes the hash table
 */
static void resize(MData *md) {
    int N;
    Node *p, *q, **array;
    long i, j;

    N = 2 * md->capacity;
    if (N > MAX_CAPACITY)
        N = MAX_CAPACITY;
    if (N == md->capacity)
        return;
    array = (Node **)malloc(N * sizeof(Node *));
    if (array == NULL)
        return;
    for (j = 0; j < N; j++)
        array[j] = NULL;
    /*
     * now redistribute the entries into the new set of buckets
     */
    for (i = 0; i < md->capacity; i++) {
        for (p = md->buckets[i]; p != NULL; p = q) {
            q = p->next;
            j = md->hash((p->entry).key, N);
            p->next = array[j];
            array[j] = p;
        }
    }
    free(md->buckets);
    md->buckets = array;
    md->capacity = N;
    md->load /= 2.0;
    md->changes = 0;
    md->increment = 1.0 / (double)N;
}

/*
 * helper function to insert new (key, value) into table
 */
static bool insertEntry(MData *md, void *key, void *value, long i) {
    Node *p = (Node *)malloc(sizeof(Node));
    bool status = (p != NULL);

    if (status) {
        (p->entry).key = key;
        (p->entry).value = value;
        p->next = md->buckets[i];
        md->buckets[i] = p;
        md->size++;
        md->load += md->increment;
        md->changes++;
    }
    return status;
}

static bool m_put(const Map *m, void *key, void *value) {
    MData *md = (MData *)m->self;
    long i;
    Node *p;
    int status = false;

    if (md->changes > TRIGGER) {
        md->changes = 0;
        if (md->load > md->loadFactor)
            resize(md);
    }
    p = findKey(md, key, &i);
    if (p != NULL) {
        md->freeK((p->entry).key);
        md->freeV((p->entry).value);
        (p->entry).key = key;
        (p->entry).value = value;
        status = true;
    } else {
        status = insertEntry(md, key, value, i);
    }
    return status;
}

static bool m_putUnique(const Map *m, void *key, void *value) {
    MData *md = (MData *)m->self;
    long i;
    Node *p;
    int status = false;

    if (md->changes > TRIGGER) {
        md->changes = 0;
        if (md->load > md->loadFactor)
            resize(md);
    }
    p = findKey(md, key, &i);
    if (p == NULL) {
        status = insertEntry(md, key, value, i);
    }
    return status;
}

static bool m_remove(const Map *m, void *key) {
    MData *md = (MData *)m->self;
    long i;
    Node *entry = findKey(md, key, &i);
    int status = (entry != NULL);

    if (status) {
        Node *p, *c;
        /* determine where the entry lives in the singly linked list */
        for (p = NULL, c = md->buckets[i]; c != entry; p = c, c = c->next)
            ;
        if (p == NULL)
            md->buckets[i] = entry->next;
        else
            p->next = entry->next;
        md->size--;
        md->load -= md->increment;
        md->changes++;
        md->freeK((entry->entry).key);
        md->freeV((entry->entry).value);
        free(entry);
    }
    return status;
}

static long m_size(const Map *m) {
    MData *md = (MData *)m->self;
    return md->size;
}

static bool m_isEmpty(const Map *m) {
    MData *md = (MData *)m->self;
    return (md->size == 0L);
}

/*
 * helper function for generating an array of keys from a map
 *
 * returns pointer to the array or NULL if malloc failure
 */
static void **keys(MData *md) {
    void **tmp = NULL;
    if (md->size > 0L) {
        size_t nbytes = md->size * sizeof(void *);
        tmp = (void **)malloc(nbytes);
        if (tmp != NULL) {
            long i, n = 0L;
            for (i = 0L; i < md->capacity; i++) {
                Node *p = md->buckets[i];
                while (p != NULL) {
                    tmp[n++] = (p->entry).key;
                    p = p->next;
                }
            }
        }
    }
    return tmp;
}

static void **m_keyArray(const Map *m, long *len) {
    MData *md = (MData *)m->self;
    void **tmp = keys(md);

    if (tmp != NULL)
        *len = md->size;
    return tmp;
}

/*
 * helper function for generating an array of MEntry * from a map
 *
 * returns pointer to the array or NULL if malloc failure
 */
static MEntry **entries(MData *md) {
    MEntry **tmp = NULL;
    if (md->size > 0L) {
        size_t nbytes = md->size * sizeof(MEntry *);
        tmp = (MEntry **)malloc(nbytes);
        if (tmp != NULL) {
            long i, n = 0L;
            for (i = 0L; i < md->capacity; i++) {
                Node *p = md->buckets[i];
                while (p != NULL) {
                    tmp[n++] = &(p->entry);
                    p = p->next;
                }
            }
        }
    }
    return tmp;
}

static MEntry **m_entryArray(const Map *m, long *len) {
    MData *md = (MData *)m->self;
    MEntry **tmp = entries(md);

    if (tmp != NULL)
        *len = md->size;
    return tmp;
}

static const Iterator *m_itCreate(const Map *m) {
    MData *md = (MData *)m->self;
    const Iterator *it = NULL;
    void **tmp = (void **)entries(md);

    if (tmp != NULL) {
        it = Iterator_create(md->size, tmp);
        if (it == NULL)
            free(tmp);
    }
    return it;
}

static const Map *m_create(const Map *m);

static Map template = {
    NULL, m_create, m_destroy, m_clear, m_containsKey, m_get, m_put,
    m_putUnique, m_remove, m_size, m_isEmpty, m_keyArray, m_entryArray,
    m_itCreate
}; 

/*
 * helper function to create a new Map dispatch table
 */
static const Map *newMap(long capacity, double loadFactor,
                         long (*hash)(void*,long), int (*cmp)(void*, void*),
                         void (*freeK)(void*), void (*freeV)(void *)) {
    Map *m = (Map *)malloc(sizeof(Map));
    long N;
    double lf;
    Node **array;
    long i;

    if (m != NULL) {
        MData *md = (MData *)malloc(sizeof(MData));

        if (md != NULL) {
            N = ((capacity > 0) ? capacity : DEFAULT_CAPACITY);
            N = (N > MAX_CAPACITY) ? MAX_CAPACITY : N;
            lf = ((loadFactor > 0.000001) ? loadFactor : DEFAULT_LOAD_FACTOR);
            array = (Node **)malloc(N * sizeof(Node *));
            if (array != NULL) {
                md->capacity = N; md->size = 0L; md->changes = 0L;
                md->loadFactor = lf; md->load = 0.0;
                md->increment = 1.0 / (double)N;
                md->hash = hash; md->cmp = cmp;
                md->freeK = freeK;
                md->freeV = freeV;
                md->buckets = array;
                for (i = 0; i < N; i++)
                    array[i] = NULL;
                *m = template;
                m->self = md;
            } else {
                free(md); free(m); m = NULL;
            }
        } else {
            free(m); m = NULL;
        }
    }
    return m;
}

static const Map *m_create(const Map *m) {
    MData *md = (MData *)m->self;

    return newMap(md->capacity, md->loadFactor, md->hash, md->cmp,
                  md->freeK, md->freeV);
}

const Map *HashMap(long capacity, double loadFactor,
                   long (*hash)(void*, long), int (*cmp)(void*, void*),
                   void (*freeK)(void *k), void (*freeV)(void *v)) {

    return newMap(capacity, loadFactor, hash, cmp, freeK, freeV);
}
