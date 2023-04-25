#ifndef _DEQUE_H_
#define _DEQUE_H_

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

typedef struct deque Deque;                  /* forward reference */

/* interface definition for generic deque implementation */

/*
 * this function signature is provided as the default constructor for any
 * Deque implementation
 *
 * freeValue is a function pointer that will be called by
 * destroy() and clear() on each entry in the Deque
 *
 * returns a pointer to the deque instance, or NULL if errors
 */
const Deque *Deque_create(void (*freeValue)(void *e));

typedef struct deque Deque;             /* forward reference */

/* now define struct deque */
struct deque {
/* the private data for the deque */
    void *self;

/* create a new deque using the same implementation as the deque upon which
 * the method has been invoked; returns NULL if error creating the new deque
 */
    const Deque *(*create)(const Deque *d);

/* destroys the deque;
 * applies constructor-specified freeValue to each element in the deque
 * the storage associated with the deque is then returned to the heap
 */
    void (*destroy)(const Deque *d);

/* clears all elements from the deque;
 * applies constructor-specified freeValue to each element in the deque
 * the deque is then re-initialized
 *
 * upon return, the deque is empty
 */
    void (*clear)(const Deque *d);

/* inserts element at the head of the deque
 *
 * returns true if successful, false if unsuccessful (malloc errors) */
    bool (*insertFirst)(const Deque *d, void *element);

/* inserts element at the tail of the deque
 *
 * returns true if successful, false if unsuccessful (malloc errors) */
    bool (*insertLast)(const Deque *d, void *element);

/* returns the element at the head of the deque in *element; does not remove it
 *
 * returns true if successful, false if the deque is empty */
    bool (*first)(const Deque *d, void **element);

/* returns the element at the tail of the deque in *element; does not remove it
 *
 * returns true if successful, false if the deque is empty */
    bool (*last)(const Deque *d, void **element);

/* removes the element at the head of the deque, returning it in *element
 *
 * returns true if successful, true if the deque is empty */
    bool (*removeFirst)(const Deque *d, void **element);

/* removes the element at the tail of the deque, returning it in *element
 *
 * returns true if successful, false if the deque is empty */
    bool (*removeLast)(const Deque *d, void **element);

/* returns the number of elements in the deque */
    long (*size)(const Deque *d);

/* returns true if the deque is empty, false if it is not */
    bool (*isEmpty)(const Deque *d);

/* returns an array containing all of the elements of the deque in
 * proper sequence (from head to tail); returns the length of the array
 * in *len
 *
 * returns a pointer to the array of void * elements, or NULL if malloc failure
 *
 * NB - the caller is responsible for freeing the void * array when finished
 * with it */
    void **(*toArray)(const Deque *d, long *len);

/* create generic iterator to this deque
 *
 * returns pointer to the Iterator or NULL if malloc failure */
    const Iterator *(*itCreate)(const Deque *d);
};

#endif /* _DEQUE_H_ */
