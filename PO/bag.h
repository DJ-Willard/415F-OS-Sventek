#ifndef _BAG_H_
#define _BAG_H_

/*
 * Copyright (c) 2021, University of Oregon
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
#include "ADTs/iterator.h"			/* needed for factory method */

/*
 * interface definition for generic bag implementation
 */

typedef struct bag Bag;  /* forward reference */

/*
 * there is no generic Bag constructor - one must invoke LListBag() or
 * HashBag() to create a Bag
 */

/*
 * now define struct bag
 */
struct bag {
/*
 * the private data of the bag
 */
    void *self;

/*
 * create a new bag with the same implementation as the bag upon which the
 * method has been invoked - the new bag will use the same freeV as the
 * bag upon which the method has been invoked.
 *
 * a pointer to the dispatch table for the new Bag is returned, or NULL if
 * there were malloc failures
 */
    const Bag *(*create)(const Bag *b);

/*
 * destroys the  bag; for each element, if the constructor-supplied
 * freeV != NULL, it is invoked on the element;
 * the storage associated with the bag is then returned to the heap
 */
    void (*destroy)(const Bag *b);

/*
 * clears all elements from the bag; for each element, if the constructor-
 * supplied freeV != NULL, it is invoked on the element;
 * any storage associated with the entry in the bag is then
 * returned to the heap
 *
 * upon return, the bag will be empty
 */
    void (*clear)(const Bag *b);

/*
 * adds the specified element to the bag
 *
 * returns 1/true if the element was added, 0/false malloc errors
 */
    bool (*add)(const Bag *b, void *element);

/*
 * returns 1/true if the bag contains the specified element, 0/false if not
 */
    bool (*contains)(const Bag *b, void *element);

/*
 * returns the number of items in the bag that are equal to element
 *
 */
    long (*count)(const Bag *b, void *element);

/*
 * removes the specified element from the bag, if present
 *
 * if the constructor-supplied freeV != NULL, invokes it on the element
 * before removing it
 *
 * returns 1/true if successful, 0/false if not present
 */
    bool (*remove)(const Bag *b, void *element);

/*
 * returns 1/true if the bag is empty, 0/false if it is not
 */
    bool (*isEmpty)(const Bag *b);

/*
 * returns the number of elements in the bag
 */
    long (*size)(const Bag *b);

/*
 * return the elements of the bag as an array of void * pointers in an
 * arbitrary order
 *
 * returns pointer to the array or NULL if error
 * returns the number of elements in the array in `*len'
 *
 * N.B. the caller is responsible for freeing the array of void* pointers
 *      when finished with it.
 */
    void **(*toArray)(const Bag *b, long *len);

/*
 * create generic iterator to the bag
 *
 * returns pointer to the Iterator or NULL if failure
 */
    const Iterator *(*itCreate)(const Bag *b);
};

#endif /* _BAG_H_ */
