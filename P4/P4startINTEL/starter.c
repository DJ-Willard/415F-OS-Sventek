#include "simplefs.h"
#include <stdlib.h>
/*
 * any other includes you require
 */

/*
 * defined constants and structure declarations you need
 */

typedef struct self {
    /* members you need in the instance-specific structure */
} Self;

/*
 * helper functions needed in your method implementations
 */

static void sf_destroy(const SimpleFS *fs) {
}

static void sf_init(const SimpleFS *fs) {
}

static bool sf_create(const SimpleFS *fs, char *name) {
    return false;
}

static bool sf_remove(const SimpleFS *fs, char *name) {
    return false;
}

static bool sf_write(const SimpleFS *fs, char *name, char *content) {
    return false;
}

static bool sf_read(const SimpleFS *fs, char *name, char *content) {
    return false;
}

static bool sf_list(const SimpleFS *fs, char *filenames) {
    return false;
}

static bool sf_info(const SimpleFS *fs, char *information) {
    return false;
}

static bool sf_dump(const SimpleFS *fs, char *dumpinfo) {
    return false;
}

static bool sf_block(const SimpleFS *fs, int block, char *blockinfo) {
    return false;
}

static const SimpleFS template = {
    NULL, sf_destroy, sf_init, sf_create, sf_remove, sf_write, sf_read,
    sf_list, sf_info, sf_dump, sf_block
};

const SimpleFS *SimpleFS_create(char *filename) {
    return NULL;
}
