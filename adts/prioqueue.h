#ifndef _PRIOQUEUE_H_
#define _PRIOQUEUE_H_

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
#include "ADTs/iterator.h"                      /* needed for factory method */

/* dispatch table structure for generic priority queue */
typedef struct prioqueue PrioQueue;

/* constructor for priority queue
 *
 * cmp is a function pointer to a comparator function between two priorities
 *
 * freePrio is a function pointer that will be called by
 * destroy() and clear() for the priority of each entry in the PrioQueue.
 *
 * freeValue is a function pointer that will be called by
 * destroy() and clear() for the value of each entry in the PrioQueue.
 *
 * returns a pointer to the priority queue, or NULL if malloc errors */
const PrioQueue *PrioQueue_create(int (*cmp)(void*, void*),
                                  void (*freePrio)(void *prio),
                                  void (*freeValue)(void *value)
                                 );

struct prioqueue {
/* the private data for the priority queue */
    void *self;

/* create a new priority queue using the same implementation as the priority
 * queue upon which the method has been invoked; returns NULL if error creating
 * the new priority queue
 */
    const PrioQueue *(*create)(const PrioQueue *pq);

/* destroys the priority queue;
 * applies the constructor-specified freePrio to each element in the prioqueue
 * applies the constructor-specified freeValue to each element in the prioqueue
 * the storage associated with the priority queue is returned to the heap */
    void (*destroy)(const PrioQueue *pq);

/* clears all elements from the priority queue;
 * applies the constructor-specified freePrio to each element in the prioqueue
 * applies the constructor-specified freeValue to each element in the prioqueue
 * the prioqueue is then re-initialized
 *
 * upon return, the priority queue is empty */
    void (*clear)(const PrioQueue *pq);

/* inserts the element into the priority queue
 *
 * returns true if successful, false if unsuccessful (malloc errors) */
    bool (*insert)(const PrioQueue *pq, void *priority, void *value);

/* returns the minimum element's priority in *priority, value *value
 *
 * returns true if successful, false if the priority queue is empty */
    bool (*min)(const PrioQueue *pq, void **priority, void **value);

/* removes the minimum element of the priority queue
 *
 * returns the value in *value
 * returns the priority in *priority
 *
 * returns true if successful, false if the priority queue is empty */
    bool (*removeMin)(const PrioQueue *pq, void **priority, void **value);

/* returns the number of elements in the priority queue */
    long (*size)(const PrioQueue *pq);

/* returns true if the priority queue is empty, false if it is not */
    bool (*isEmpty)(const PrioQueue *pq);

/* returns an array containing all of the values of the priority queue in
 * proper sequence (smallest priority to largest priority); returns the
 * length of the array in *len
 *
 * returns a pointer to the array of void * values, or NULL if malloc failure
 *
 * NB - the caller is responsible for freeing the void * array when finished
 * with it */
    void **(*toArray)(const PrioQueue *pq, long *len);

/* create generic iterator to this priority queue
 *
 * returns pointer to the Iterator or NULL if malloc failure */
    const Iterator *(*itCreate)(const PrioQueue *pq);
};

#endif /* _PRIOQUEUE_H_ */
