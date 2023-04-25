#include "llistset.h"  /* the .h file does NOT reside in /usr/local/include/ADTs */
#include <stdlib.h>
/* any other includes needed by your code */
#define UNUSED __attribute__((unused))

typedef struct s_data {
    /* definitions of the data members of self */
} SData;

/*
 * important - remove UNUSED Attribute in signatures when you flesh out methods
 */

static void s_destroy(UNUSED const Set *s) {
    /* implement the destroy() method */
    return;
}

static void s_clear(UNUSED const Set *s) {
    /* implement the clear() method */
    return;
}

static bool s_add(UNUSED const Set *s, UNUSED void *member) {
    /* implement the add() method */
    return 0;
}

static bool s_contains(UNUSED const Set *s, UNUSED void *member) {
    /* implement the contains() method */
    return 0;
}

static bool s_isEmpty(UNUSED const Set *s) {
    /* implement the isEmpty() method */
    return 1;
}

static bool s_remove(UNUSED const Set *s, UNUSED void *member) {
    /* implement the remove() method */
    return 0;
}

static long s_size(UNUSED const Set *s) {
    /* implement the size() method */
    return 0L;
}

static void **s_toArray(UNUSED const Set *s, UNUSED long *len) {
    /* implement the toArray() method */
    return NULL;
}

static const Iterator *s_itCreate(UNUSED const Set *s) {
    /* implement the itCreate() method */
    return NULL;
}

static UNUSED Set template = {
    NULL, s_destroy, s_clear, s_add, s_contains, s_isEmpty, s_remove,
    s_size, s_toArray, s_itCreate
};

const Set *LListSet(UNUSED void (*freeValue)(void*), UNUSED int (*cmpFxn)(void*, void*)) {
    /* construct a Set instance and return to the caller */
    return NULL;
}
