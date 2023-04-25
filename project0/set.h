#ifndef _SET_H_
#define _SET_H_

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

#include "ADTs/iterator.h"			/* needed for factory method */

/*
 * interface definition for generic set implementation
 */

typedef struct set Set;  /* forward reference */

/*
 * now define struct set
 */
struct set {
/*
 * the private data of the set
 */
    void *self;

/*
 * destroys the  set; applies freeValue function to each element
 * the storage associated with the set is then returned to the heap
 */
    void (*destroy)(const Set *s);

/*
 * clears all elements from the set; applies freeValue function to each element
 *
 * upon return, the set will be empty
 */
    void (*clear)(const Set *s);

/*
 * adds the specified element to the set if it is not already present
 *
 * returns 1 if the element was added, 0 if the element was already present
 */
    bool (*add)(const Set *s, void *element);

/*
 * returns 1 if the set contains the specified element, 0 if not
 */
    bool (*contains)(const Set *s, void *element);

/*
 * returns 1 if set is empty, 0 if it is not
 */
    bool (*isEmpty)(const Set *s);

/*
 * removes the specified element from the set, if present
 *
 * applies freeValue function to element before removing it from the set
 *
 * returns 1 if successful, 0 if not present
 */
    bool (*remove)(const Set *s, void *element);

/*
 * returns the number of elements in the set
 */
    long (*size)(const Set *s);

/*
 * return the elements of the set as an array of void * pointers in an
 * arbitrary order
 *
 * returns pointer to the array or NULL if error
 * returns the number of elements in the array in `*len'
 *
 * N.B. the caller is responsible for freeing the array of void* pointers
 *      when finished with it.
 */
    void **(*toArray)(const Set *s, long *len);

/*
 * create generic iterator to this hashset
 *
 * returns pointer to the Iterator or NULL if failure
 */
    const Iterator *(*itCreate)(const Set *s);
};

#endif /* _SET_H_ */
