#ifndef _FREESECTORDESCRIPTORSTORE__FULL_H_
#define _FREESECTORDESCRIPTORSTORE__FULL_H_

/*
 * This file is a component of the test harness and/or sample solution
 * to the DiskDriver exercise used in CIS415, Project 2, at the Univ of Oregon
 */

/*
 * extension to header file for FreeSectorDescriptorStore ADT to
 * define constructor and destructor
 */

#include "freesectordescriptorstore.h"

FreeSectorDescriptorStore *FreeSectorDescriptorStore_create(
                               void *mem_start, unsigned long mem_length);
void FreePacketDescriptorStore_destroy(FreeSectorDescriptorStore *fsds);

#endif /* _FREESECTORDESCRIPTORSTORE__FULL_H_ */
