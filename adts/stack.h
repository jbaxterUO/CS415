#ifndef _STACK_H_
#define _STACK_H_

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
 * interface definition for generic stack
 *
 * patterned roughly after Java 6 Stack interface
 */

#include "ADTs/ADTdefs.h"
#include "ADTs/iterator.h"                   /* needed for factory method */

typedef struct stack Stack;                /* forward reference */

/*
 * this function signature is provided as the default constructor for
 * any Stack implementation
 *
 * freeValue is a function pointer that will be called by
 * destroy() and clear() on each entry in the Stack
 *
 * returns a pointer to the stack instance, or NULL if errors
 */
const Stack *Stack_create(void (*freeValue)(void *e));

/*
 * dispatch table for a generic stack
 */
struct stack {
/*
 * the private data of the stack
 */
    void *self;

/*
 * create a new stack using the same implementation as the stack upon which
 * the method has been invoked; returns NULL if error creating the new stack
 */
    const Stack *(*create)(const Stack *st);

/*
 * destroys the stack;
 * applies constructor-specified freeValue to each element in the stack
 * the storage associated with the stack is then returned to the heap
 */
    void (*destroy)(const Stack *st);

/*
 * clears all elements from the stack;
 * applies constructor-specified freeValue to each element in the stack
 * the stack is then re-initialized
 *
 * upon return, the stack is empty
 */
    void (*clear)(const Stack *st);

/*
 * pushes `element' onto the stack; if no more room in the stack, it is
 * dynamically resized
 *
 * returns true if successful, false if unsuccessful (malloc errors)
 */
    bool (*push)(const Stack *st, void *element);

/*
 * pops the element at the top of the stack into `*element'
 *
 * returns true if successful, false if stack was empty
 */
    bool (*pop)(const Stack *st, void **element);

/*
 * peeks at the top element of the stack without removing it;
 * returned in `*element'
 *
 * return true if successful, false if stack was empty
 */
    bool (*peek)(const Stack *st, void **element);

/*
 * returns the number of elements in the stack
 */
    long (*size)(const Stack *st);

/*
 * returns true if the stack is empty, false if not
 */
    bool (*isEmpty)(const Stack *st);

/*
 * returns an array containing all of the elements of the stack in
 * proper sequence (from top to bottom element); returns the length of the
 * array in `*len'
 *
 * returns pointer to array of void * elements, or NULL if malloc failure
 *
 * The array of void * pointers is allocated on the heap, so must be returned
 * by a call to free() when the caller has finished using it.
 */
    void **(*toArray)(const Stack *st, long *len);

/*
 * creates generic iterator to this stack;
 * successive next calls return elements in the proper sequence (top to bottom)
 *
 * returns pointer to the Iterator or NULL if malloc failure
 */
    const Iterator *(*itCreate)(const Stack *st);
};

#endif /* _STACK_H_ */
