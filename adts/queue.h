#ifndef _QUEUE_H_
#define _QUEUE_H_

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
 * interface definition for generic FIFO queue
 *
 * patterned roughly after Java 6 Queue interface
 */

#include "ADTs/ADTdefs.h"
#include "ADTs/iterator.h"              /* needed for factory method */

typedef struct queue Queue;        /* forward reference */

/*
 * this function signature is provided as the default constructor for
 * any Queue implementation
 *
 * freeValue is a function pointer that will be called by
 * destroy() and clear() on each entry in the Stack
 *
 * returns a pointer to the stack instance, or NULL if errors
 */
const Queue *Queue_create(void (*freeValue)(void *e));

/*
 * dispatch table for a generic Queue
 */
struct queue {
/*
 * the private data of the queue
 */
    void *self;

/*
 * create a new queue using the same implementation as the queue upon which
 * the method has been invoked; returns NULL if error creating the queue
 */
    const Queue *(*create)(const Queue *q);

/*
 * destroys the queue;
 * applies constructor-specified freeValue to each element in the queue
 * the storage associated with the queue is then returned to the heap
 */
    void (*destroy)(const Queue *q);

/*
 * clears all elements from the queue;
 * applies constructor-specified freeValue to each element in the queue
 * the queue is then re-initialized
 *
 * upon return, the queue is empty
 */
    void (*clear)(const Queue *q);

/*
 * appends `element' to the end of the queue
 *
 * returns true if successful, false if unsuccesful (malloc failure)
 */
    bool (*enqueue)(const Queue *q, void *element);

/*
 * retrieves, but does not remove, the head of the queue, returning that
 * element in `*element'
 *
 * returns true if successful, false if unsuccessful (queue is empty)
 */
    bool (*front)(const Queue *q, void **element);

/*
 * Retrieves, and removes, the head of the queue, returning that
 * element in `*element'
 *
 * return true if successful, false if not (queue is empty)
 */
    bool (*dequeue)(const Queue *q, void **element);

/*
 * returns the number of elements in the queue
 */
    long (*size)(const Queue *q);

/*
 * returns true if the queue is empty, false if not
 */
    bool (*isEmpty)(const Queue *q);

/*
 * returns an array containing all of the elements of the queue in
 * proper sequence (from first to last element); returns the length of the
 * queue in `*len'
 *
 * returns pointer to array of void * elements, or NULL if malloc failure
 *
 * NB - it is the caller's responsibility to free the void * array when
 *      finished with it
 */
    void **(*toArray)(const Queue *q, long *len);

/*
 * creates an iterator for running through the queue
 *
 * returns pointer to the Iterator or NULL
 */
    const Iterator *(*itCreate)(const Queue *q);
};

#endif /* _QUEUE_H_ */
