#ifndef _CSKMAP_H_
#define _CSKMAP_H_

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

#include "ADTs/ADTdefs.h"
#include "ADTs/iterator.h"               /* needed for factory method */

/* interface definition for C string key map */

typedef struct cskmap CSKMap;             /* forward reference */
typedef struct mentry {
    char *key;
    void *value;
} MEntry;

/*
 * constructor for a CSKMap
 *
 * freeValue() is applied by destroy(), clear(), put(), and remove()
 * to appropriate element[s]
 *
 * returns pointer to dispatch table, or NULL if malloc failures
 */
const CSKMap *CSKMap_create(void (*freeValue)(void *v));

/* now define struct cskmap */
struct cskmap {
/* the private data for the map */
    void *self;

/* create a new map using the same implementation as the map upon which
 * the method has been invoked; returns NULL if error creating the new map
 */
    const CSKMap *(*create)(const CSKMap *m);

/* destroys the map;
 * applies constructor-specified freeValue to each element in the map
 * the storage associated with the map is returned to the heap */
    void (*destroy)(const CSKMap *m);

/* clears all (key,value) pairs from the map;
 * applies constructor-specified freeValue to each element in the map
 * the map is then re-initialized
 *
 * upon return, the map is empty */
    void (*clear)(const CSKMap *m);

/* returns true if key is contained in the map, false if not */
    bool (*containsKey)(const CSKMap *m, char *key);

/* returns the value associated with key in *value; returns true if key was
 * found in the map, false if not */
    bool (*get)(const CSKMap *m, char *key, void **value);

/* puts (key,value) into the map;
 * applies constructor-specified freeValue if there was a previous entry
 *
 * returns true if (key,value) was successfully stored in the map,
 * false if not */
    bool (*put)(const CSKMap *m, char *key, void *value);

/* puts (key,value) into the map iff the map does not contain a value associated
 * with key
 *
 * returns true if (key,value) was successfully stored in the map,
 * false if not */
    bool (*putUnique)(const CSKMap *m, char *key, void *value);

/* removes the (key,value) pair from the map;
 * applies constructor-specified freeValue to the removed entry
 *
 * returns true if (key,value) was present and removed,
 * false if it was not present */
    bool (*remove)(const CSKMap *m, char *key);

/* returns the number of (key,value) pairs in the map */
    long (*size)(const CSKMap *m);

/* returns true if the map is empty, false if not */
    bool (*isEmpty)(const CSKMap *m);

/* returns an array containing all of the keys in the map; the order is
 * arbitrary; returns the length of the array in *len
 *
 * returns a pointer to the char * array of keys, or NULL if malloc failure
 *
 * NB - the caller is responsible for freeing the char * array when finished
 * with it */
    char **(*keyArray)(const CSKMap *m, long *len);

/* returns an array containing all of the (key,value) pairs in the map;
 * the order is arbitrary; returns the length of the array in *len
 *
 * returns a pointer to the MEntry * array of (k,v) pairs,
 * or NULL if malloc failure
 *
 * NB - the caller is responsible for freeing the MEntry * array when finished
 * with it */
    MEntry **(*entryArray)(const CSKMap *m, long *len);

/* create generic iterator to the map
 *
 * returns pointer to the Iterator or NULL if malloc failure
 *
 * NB - when the next() method on the iterator is called, it returns an
 *      MEntry *
 */
    const Iterator *(*itCreate)(const CSKMap *m);
};

#endif /* _CSKMAP_H_ */
