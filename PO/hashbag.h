#ifndef _HASHBAG_H_
#define _HASHBAG_H_

/*
 * Copyright (c) 2019, University of Oregon
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

#include "bag.h"

/*
 * constructor for a HashBag
 */

/*
 * create a HashBag
 *
 * cmpF is used to determine equality between two objects, with
 * `cmpF(first, second)' returning 0 if first==second, <>0 otherwise
 *
 * if capacity == 0L, the initial capacity is set to DEFAULT_CAPACITY
 *
 * if loadFactor == 0.0, a default load factor (0.75) is used
 * if number of elements/number of buckets exceeds the load factor, the
 * table must be resized, doubling the number of buckets, up to a max
 * number of buckets (134,217,728)
 *
 * hashF is used to hash a value into the hash table that underlies
 * the bag, with `hashF(value, N)' returning a number in [0,N)
 *
 * returns a pointer to the bag, or NULL if there are malloc() errors
 */
#define DEFAULT_CAPACITY 16L
#define DEFAULT_LOAD_FACTOR 0.75

const Bag *HashBag(int (*cmpF)(void*, void*), void freeV(void*),
                   long capacity, double loadFactor,
                   long (*hashFunction)(void*, long)
                  );

#endif /* _HASHBAG_H_ */
