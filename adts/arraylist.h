#ifndef _ARRAYLIST_H_
#define _ARRAYLIST_H_

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
#include "ADTs/iterator.h"                   /* needed for factory method */

/* interface definition for generic arraylist implementation
 *
 * patterned roughly after Java 6 ArrayList generic class
 */

typedef struct arraylist ArrayList;     /* forward reference */

#define DEFAULT_ARRAYLIST_CAPACITY 50L

/* create an arraylist with the specified capacity; if capacity == 0, a
 * default initial capacity (50 elements) is used
 *
 * freeValue is a function pointer that will be called by
 * destroy(), clear(), remove(), and set() on each relevant entry/entries
 * in the ArrayList
 *
 * returns a pointer to the array list, or NULL if there are malloc() errors
 */
const ArrayList *ArrayList_create(long capacity, void (*freeValue)(void *e));

/* now define the dispatch table */
struct arraylist {
/* the private data of the array list */
    void *self;

/* destroys the arraylist; 
 * applies constructor-specified freeValue to each element in the arraylist
 * the storage associated with the arraylist is then returned to the heap
 */
    void (*destroy)(const ArrayList *al);

/* appends `element' to the arraylist; if no more room in the list, it is
 * dynamically resized
 *
 * returns true if successful, false if unsuccessful (malloc errors)
 *
 */
    bool (*add)(const ArrayList *al, void *element);

/* clears all elements from the arraylist;
 * applies constructor-specified freeValue to each element in the arraylist
 * any storage associated with each element in the arraylist is then
 * returned to the heap
 *
 * upon return, the arraylist will be empty
 */
    void (*clear)(const ArrayList *al);

/* ensures that the arraylist can hold at least `minCapacity' elements
 *
 * returns true if successful, false if unsuccessful (malloc failure)
 */
    bool (*ensureCapacity)(const ArrayList *al, long minCapacity);

/* returns the element at the specified position in this list in `*element'
 *
 * returns true if successful, false if no element at that position
 */
    bool (*get)(const ArrayList *al, long index, void **element);

/* inserts `element' at the specified position in the arraylist;
 * all elements from `i' onwards are shifted one position to the right;
 * if no more room in the list, it is dynamically resized;
 * if the current size of the list is N, legal values of i are in the
 * interval [0, N]
 *
 * returns true if successful, false if unsuccessful (malloc errors)
 */
    bool (*insert)(const ArrayList *al, long index, void *element);

/* returns true if arraylist is empty, false if it is not */
    bool (*isEmpty)(const ArrayList *al);

/* removes the `i'th element from the list
 * all elements from [i+1, size-1] are shifted down one position
 * applies constructor-specified freeValue to the removed element
 *
 * returns true if successful, false if `i'th position was not occupied
 */
    bool (*remove)(const ArrayList *al, long index);

/* relaces the `i'th element of the arraylist with `element';
 * applies constructor-specified freeValue to the replaced element
 *
 * returns true if successful
 * returns false if `i'th position not currently occupied
 */
    bool (*set)(const ArrayList *al, long index, void *element);

/* returns the number of elements in the arraylist */
    long (*size)(const ArrayList *al);

/* returns an array containing all of the elements of the list in
 * proper sequence (from first to last element); returns the length of
 * the list in `*len'
 *
 * returns pointer to void * array of elements, or NULL if malloc failure
 *
 * NB - the caller is responsible for freeing the void * array when finished
 *      with it
 */
    void **(*toArray)(const ArrayList *al, long *len);

/* trims the capacity of the arraylist to be the list's current size
 *
 * returns true if successful, false if failure (malloc errors)
 */
    bool (*trimToSize)(const ArrayList *al);

/* create generic iterator to this arraylist
 *
 * returns pointer to the Iterator or NULL if failure
 */
    const Iterator *(*itCreate)(const ArrayList *al);
};

#endif /* _ARRAYLIST_H_ */
