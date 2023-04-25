#ifndef _HASHSET_H_
#define _HASHSET_H_

/*
 * Copyright (c) 2020, 2021, University of Oregon
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
 * constructor definition for generic hashset implementation
 */

#include "set.h"            /* dispatch table */


/*
 * create a HashSet
 *
 * freeValue is a function that returns any heap memory associated with
 * a set element to the heap; if specified as doNothing, nothing is done to
 * the elements before they are deleted
 *
 * cmp is used to determine equality between two objects, with
 * `cmp(first, second)' returning 0 if first==second, <>0 otherwise
 *
 * if capacity == 0L, the initial capacity is set to DEFAULT_SET_CAPACITY
 *
 * if loadFactor == 0.0, a default load factor (0.75) is used
 * if number of elements/number of buckets exceeds the load factor, the
 * table must be resized, doubling the number of buckets, up to a max
 * number of buckets (134,217,728)
 *
 * hashFunction is used to hash a value into the hash table that underlies
 * the set, with `hashFunction(value, N)' returning a number in [0,N)
 *
 * returns a pointer to the set, or NULL if there are malloc() errors
 */
#define DEFAULT_SET_CAPACITY 16L
#define DEFAULT_LOAD_FACTOR 0.75

const Set *HashSet(void (*freeValue)(void *), int (*cmp)(void*, void*),
                   long capacity, double loadFactor,
                   long (*hashFunction)(void *, long)
                  );

#endif /* _HASHSET_H_ */
