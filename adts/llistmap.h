#ifndef _LLISTMAP_H_
#define _LLISTMAP_H_

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

#include "ADTs/ADTdefs.h"
#include "ADTs/map.h"

/* constructor for generic linked list map */

/* create a linked list map
 *
 * returns a pointer to the linked list map, or NULL if there are malloc errors
 *
 * the cmp function pointer is applied to a pair of keys, yielding <0 | 0 | >0
 *
 * freeK is a function pointer that will be called by destroy(),
 * clear(), put(), and remove() on keys of relevant entry/entries in the Map
 *
 * freeV is a function pointer that will be called by destroy(),
 * clear(), put(), and remove() on values of relevant entry/entries in the Map
 */
const Map *LListMap(int (*cmp)(void*, void*), void (*freeK)(void *k),
                    void (*freeV)(void *v));

#endif /* _LLISTMAP_H_ */
