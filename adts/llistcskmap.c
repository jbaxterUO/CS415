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
 * implementation for C string key linked list map
 */

#include "ADTs/llistcskmap.h"
#include <stdlib.h>
#include <string.h>

typedef struct node {
    struct node *next;
    struct node *prev;
    MEntry entry;
} Node;

typedef struct m_data {
    long size;
    Node sentinel;
    void (*freeValue)(void *v);
} MData;

/*
 * traverses the map, calling freeValue on each entry
 * then frees storage associated with the Node structure
 */
static void purge(MData *md) {
    Node *p, *q = NULL;

    for (p = md->sentinel.next; p != &(md->sentinel); p = q) {
        free((p->entry).key);
        md->freeValue((p->entry).value);
        q = p->next;
        free(p);
    }
}

static void m_destroy(const CSKMap *m) {
    MData *md = (MData *)m->self;
    purge(md);
    free(md);
    free((void *)m);
}

static void m_clear(const CSKMap *m) {
    MData *md = (MData *)m->self;
    purge(md);
    md->size = 0L;
    md->sentinel.next = md->sentinel.prev = &(md->sentinel);
}

/*
 * helper function to locate key in a map
 *
 * returns pointer to entry, if found, as function value; NULL if not found
 */
static Node *findKey(MData *md, char *key) {
    Node *p;

    for (p = md->sentinel.next; p != &(md->sentinel); p = p->next) {
        if (strcmp((p->entry).key, key) == 0) {
            break;
        }
    }
    return (p != &(md->sentinel)) ? p : NULL;
}

static bool m_containsKey(const CSKMap *m, char *key) {
    MData *md = (MData *)m->self;

    return (findKey(md, key) != NULL);
}

static bool m_get(const CSKMap *m, char *key, void **value) {
    MData *md = (MData *)m->self;
    Node *p = findKey(md, key);
    bool status = (p != NULL);

    if (status)
        *value = (p->entry).value;
    return status;
}

/*
 * helper function to link `p' between `before' and `after'
 */
static void link(Node *before, Node *p, Node *after) {
    p->next = after;
    p->prev = before;
    after->prev = p;
    before->next = p;
}

/*
 * helper function to insert new (key, value) into table
 */
static bool insertEntry(MData *md, char *key, void *value) {
    Node *p = (Node *)malloc(sizeof(Node));
    bool status = (p != NULL);

    if (status) {
        char *k = strdup(key);
        status = (k != NULL);
        if (status) {
            (p->entry).key = k;
            (p->entry).value = value;
            link(md->sentinel.prev, p, &(md->sentinel));
            md->size++;
        } else {
            free(p);
        }
    }
    return status;
}

static bool m_put(const CSKMap *m, char *key, void *value) {
    MData *md = (MData *)m->self;
    Node *p = findKey(md, key);
    bool status = (p != NULL);

    if (status) {
        md->freeValue((p->entry).value);
        (p->entry).value = value;
    } else {
        status = insertEntry(md, key, value);
    }
    return status;
}

static bool m_putUnique(const CSKMap *m, char *key, void *value) {
    MData *md = (MData *)m->self;
    Node *p = findKey(md, key);
    bool status = (p == NULL);

    if (status) {
        status = insertEntry(md, key, value);
    }
    return status;
}

/*
 * unlinks `p' from the doubly-linked list
 */
static void unlink(Node *p) {
    p->prev->next = p->next;
    p->next->prev = p->prev;
}

static bool m_remove(const CSKMap *m, char *key) {
    MData *md = (MData *)m->self;
    Node *p = findKey(md, key);
    bool status = (p != NULL);

    if (status) {
        unlink(p);
        md->size--;
        free((p->entry).key);
        md->freeValue((p->entry).value);
        free(p);
    }
    return status;
}

static long m_size(const CSKMap *m) {
    MData *md = (MData *)m->self;
    return md->size;
}

static bool m_isEmpty(const CSKMap *m) {
    MData *md = (MData *)m->self;
    return (md->size == 0L);
}

/*
 * helper function for generating an array of keys from a map
 *
 * returns pointer to the array or NULL if malloc failure
 */
static char **keys(MData *md) {
    char **tmp = NULL;
    if (md->size > 0L) {
        size_t nbytes = md->size * sizeof(char *);
        tmp = (char **)malloc(nbytes);
        if (tmp != NULL) {
            long n = 0L;
            Node *p;
            for (p = md->sentinel.next; p != &(md->sentinel); p = p->next)
                tmp[n++] = (p->entry).key;
        }
    }
    return tmp;
}

static char **m_keyArray(const CSKMap *m, long *len) {
    MData *md = (MData *)m->self;
    char **tmp = keys(md);

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
            long n = 0L;
            Node *p;
            for (p = md->sentinel.next; p != &(md->sentinel); p = p->next)
                tmp[n++] = &(p->entry);
        }
    }
    return tmp;
}

static MEntry **m_entryArray(const CSKMap *m, long *len) {
    MData *md = (MData *)m->self;
    MEntry **tmp = entries(md);

    if (tmp != NULL)
        *len = md->size;
    return tmp;
}

static const Iterator *m_itCreate(const CSKMap *m) {
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

static const CSKMap *m_create(const CSKMap *m);

static CSKMap template = {
    NULL, m_create, m_destroy, m_clear, m_containsKey, m_get, m_put,
    m_putUnique, m_remove, m_size, m_isEmpty, m_keyArray, m_entryArray,
    m_itCreate
}; 

/*
 * helper function to create a new CSKMap dispatch table
 */
static const CSKMap *newCSKMap(void (*freeValue)(void *)) {
    CSKMap *m = (CSKMap *)malloc(sizeof(CSKMap));

    if (m != NULL) {
        MData *md = (MData *)malloc(sizeof(MData));

        if (md != NULL) {
            md->size = 0L;
            md->sentinel.next = md->sentinel.prev = &(md->sentinel);
            md->freeValue = freeValue;
            *m = template;
            m->self = md;
        } else {
            free(m);
            m = NULL;
        }
    }
    return m;
}

static const CSKMap *m_create(const CSKMap *m) {
    MData *md = (MData *)m->self;

    return newCSKMap(md->freeValue);
}

const CSKMap *CSKMap_create(void (*freeValue)(void *v)) {
    return newCSKMap(freeValue);
}

const CSKMap *LListCSKMap(void (*freeValue)(void *v)) {
    return newCSKMap(freeValue);
}
