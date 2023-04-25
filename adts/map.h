#ifndef _MAP_H_
#define _MAP_H_

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

/* interface definition for generic map implementation */

typedef struct map Map;             /* forward reference */
typedef struct mentry {
    void *key;
    void *value;
} MEntry;

/* now define struct map */
struct map {
/* the private data for the map */
    void *self;

/* create a new map using the same implementation as the map upon which
 * the method has been invoked; returns NULL if error creating the new map
 */
    const Map *(*create)(const Map *m);

/* destroys the map;
 * applies constructor-specified freeK and freeV to each element in the map
 * the storage associated with the map is returned to the heap */
    void (*destroy)(const Map *m);

/* clears all (key,value) pairs from the map;
 * applies constructor-specified freeK and freeV to each element in the map
 * the map is then re-initialized
 *
 * upon return, the map is empty */
    void (*clear)(const Map *m);

/* returns true if key is contained in the map, false if not */
    bool (*containsKey)(const Map *m, void *key);

/* returns the value associated with key in *value; returns true if key was
 * found in the map, false if not */
    bool (*get)(const Map *m, void *key, void **value);

/* puts (key,value) into the map;
 * applies constructor-specified freeK and freeV if there was a previous entry
 *
 * returns true if (key,value) was successfully stored in the map,
 * false if not */
    bool (*put)(const Map *m, void *key, void *value);

/* puts (key,value) into the map iff the map does not contain a value associated
 * with key
 *
 * returns true if (key,value) was successfully stored in the map,
 * false if not */
    bool (*putUnique)(const Map *m, void *key, void *value);

/* removes the (key,value) pair from the map;
 * applies constructor-specified freeK and freeV to the removed entry
 *
 * returns true if (key,value) was present and removed,
 * false if it was not present */
    bool (*remove)(const Map *m, void *key);

/* returns the number of (key,value) pairs in the map */
    long (*size)(const Map *m);

/* returns true if the map is empty, false if not */
    bool (*isEmpty)(const Map *m);

/* returns an array containing all of the keys in the map; the order is
 * arbitrary; returns the length of the array in *len
 *
 * returns a pointer to the array of void * keys, or NULL if malloc failure
 *
 * NB - the caller is responsible for freeing the void * array when finished
 * with it */
    void **(*keyArray)(const Map *m, long *len);

/* returns an array containing all of the (key,value) pairs in the map;
 * the order is arbitrary; returns the length of the array in *len
 *
 * returns a pointer to the MEntry * array of (k,v) pairs,
 * or NULL if malloc failure
 *
 * NB - the caller is responsible for freeing the MEntry * array when finished
 * with it */
    MEntry **(*entryArray)(const Map *m, long *len);

/* create generic iterator to the map
 *
 * returns pointer to the Iterator or NULL if malloc failure
 *
 * NB - when the next() method on the iterator is called, it returns an
 *      MEntry *
 */
    const Iterator *(*itCreate)(const Map *m);
};

#endif /* _MAP_H_ */
