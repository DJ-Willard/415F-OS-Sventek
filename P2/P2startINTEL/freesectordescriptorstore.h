#ifndef _FREESECTORDESCRIPTORSTORE_H_
#define _FREESECTORDESCRIPTORSTORE_H_

/*
 * This file is a component of the test harness and/or sample solution
 * to the DiskDriver exercise used in CIS415, Project 2, at the Univ of Oregon
 */

/*
 * header file for FreeSectorDescriptorStore ADT
 */

#include "sectordescriptor.h"

/*
 * as usual, the blocking versions only return when they succeed
 * the nonblocking versions return 1 if successful, 0 otherwise
 * the Get methods set their final argument if they succeed
 */

typedef struct free_sector_descriptor_store FreeSectorDescriptorStore;
struct free_sector_descriptor_store {
    void *self;
    void (*blockingGet)(FreeSectorDescriptorStore *fsds, SectorDescriptor **sd);
    int (*nonblockingGet)(FreeSectorDescriptorStore *fsds, SectorDescriptor **sd);
    void (*blockingPut)(FreeSectorDescriptorStore *fsds, SectorDescriptor *sd);
    int (*nonblockingPut)(FreeSectorDescriptorStore *fsds, SectorDescriptor *sd);
    unsigned long (*size)(FreeSectorDescriptorStore *fsds);
};

#endif /* _FREESECTORDESCRIPTORSTORE_H_ */
