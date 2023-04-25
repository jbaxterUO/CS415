#include "hashset.h"
#include "llistset.h"
#include "sort.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int ncmp(void *x1, void *x2) {
    return ((long)x1 - (long)x2);
}

int scmp(void *x1, void *x2) {
    return strcmp((char *)x1, (char *)x2);
}

long nhash(void *x, long N) {
    return ((long)x) % N;
}

#define A 31L
long shash(void *x, long N) {
    int i;
    long sum = 0L;
    char *s = (char *)x;

    for (i = 0; s[i] != '\0'; i++)
        sum = A * sum + (long)s[i];
    return sum % N;
}

int findLong(long v, long array[], long size) {
    long i;
    for (i = 0L; i < size; i++)
        if (array[i] == v)
            return 1;
    return 0;
}

int findStr(char *v, char *array[], long size) {
    long i;
    for (i = 0L; i < size; i++)
        if (strcmp(v, array[i]) == 0)
            return 1;
    return 0;
}

const Set *create(int dohash, void (*freeV)(void*), int (*cmp)(void*, void*),
                  long cap, double lf, long (*hash)(void*, long)) {
    const Set *s;
    if (dohash)
        s = HashSet(freeV, cmp, cap, lf, hash);
    else
        s = LListSet(freeV, cmp);
    return s;
}

#define HASH 1
#define LLIST 2
#define USAGE "usage: %s -h|-l test# . . .\n"

int main(int argc, char *argv[]) {
    int i;
    int which = 0;
    int opt;
    int dohash;

    opterr = 0;
    while ((opt = getopt(argc, argv, "hl")) != -1) {
        switch (opt) {
        case 'h': which = HASH; break;
        case 'l': which = LLIST; break;
        default:
            fprintf(stderr, "%s: illegal option, '-%c'\n", argv[0], optopt);
            fprintf(stderr, USAGE, argv[0]);
            return EXIT_FAILURE;
            break;
        }
    }
    if (which == 0) {
        fprintf(stderr, "%s: you must specify either -h or -l\n", argv[0]);
        fprintf(stderr, USAGE, argv[0]);
        return EXIT_FAILURE;
    }
    dohash = (which == HASH);
    for (i = optind; i < argc; i++) {
        int test;
        sscanf(argv[i], "%d", &test);
        switch(test) {
          case 1: {
            printf("Test creation and destruction of a set ... ");
            const Set *s = create(dohash, doNothing, ncmp, 0L, 0.0, nhash);
            if (s != NULL) {
                printf("success\n");
                s->destroy(s);
            } else
                printf("failure\n");
            break;
          }
          case 2: {
            printf("Test addition of a single value ... ");
            const Set *s = create(dohash, doNothing, ncmp, 0L, 0.0, nhash);
            if (s->add(s, (void *)42))
                printf("success\n");
            else
                printf("failure\n");
            s->destroy(s);
            break;
          }
          case 3: {
            printf("Test addition of a duplicate value ... ");
            const Set *s = create(dohash, doNothing, ncmp, 0L, 0.0, nhash);
            (void)s->add(s, (void *)42);
            if (! s->add(s, (void *)42))
                printf("success\n");
            else
                printf("failure\n");
            s->destroy(s);
            break;
          }
          case 4: {
            printf("Test isEmpty() on an empty set ... ");
            const Set *s = create(dohash, doNothing, ncmp, 0L, 0.0, nhash);
            if (s->isEmpty(s))
                printf("success\n");
            else
                printf("failure\n");
            s->destroy(s);
            break;
          }
          case 5: {
            printf("Test isEmpty() on a non-empty set ... ");
            const Set *s = create(dohash, doNothing, ncmp, 0L, 0.0, nhash);
            (void)s->add(s, (void *)42);
            if (! s->isEmpty(s))
                printf("success\n");
            else
                printf("failure\n");
            s->destroy(s);
            break;
          }
          case 6: {
            printf("Test contains() on an empty set ... ");
            const Set *s = create(dohash, doNothing, ncmp, 0L, 0.0, nhash);
            if (! s->contains(s, (void *)42))
                printf("success\n");
            else
                printf("failure\n");
            s->destroy(s);
            break;
          }
          case 7: {
            printf("Test contains() on a non-empty set and value is present ... ");
            const Set *s = create(dohash, doNothing, ncmp, 0L, 0.0, nhash);
            (void)s->add(s, (void *)42);
            if (s->contains(s, (void *)42))
                printf("success\n");
            else
                printf("failure\n");
            s->destroy(s);
            break;
          }
          case 8: {
            printf("Test contains() on a non-empty set and value is not present ... ");
            const Set *s = create(dohash, doNothing, ncmp, 0L, 0.0, nhash);
            (void)s->add(s, (void *)42);
            if (! s->contains(s, (void *)99))
                printf("success\n");
            else
                printf("failure\n");
            s->destroy(s);
            break;
          }
          case 9: {
            printf("Test remove() on an empty set ... ");
            const Set *s = create(dohash, doNothing, ncmp, 0L, 0.0, nhash);
            if (! s->remove(s, (void *)99))
                printf("success\n");
            else
                printf("failure\n");
            s->destroy(s);
            break;
          }
          case 10: {
            printf("Test remove() on a non-empty set and value is present ... ");
            const Set *s = create(dohash, doNothing, ncmp, 0L, 0.0, nhash);
            (void)s->add(s, (void *)42);
            if (s->remove(s, (void *)42))
                printf("success\n");
            else
                printf("failure\n");
            s->destroy(s);
            break;
          }
          case 11: {
            printf("Test remove() on a non-empty set and value is not present ... ");
            const Set *s = create(dohash, doNothing, ncmp, 0L, 0.0, nhash);
            (void)s->add(s, (void *)42);
            if (! s->remove(s, (void *)99))
                printf("success\n");
            else
                printf("failure\n");
            s->destroy(s);
            break;
          }
          case 12: {
            printf("Test size() on an empty set ... ");
            const Set *s = create(dohash, doNothing, ncmp, 0L, 0.0, nhash);
            if (s->size(s) == 0L)
                printf("success\n");
            else
                printf("failure\n");
            s->destroy(s);
            break;
          }
          case 13: {
            printf("Test size() on a non-empty set ... ");
            const Set *s = create(dohash, doNothing, ncmp, 0L, 0.0, nhash);
            (void)s->add(s, (void *)42);
            if (s->size(s) != 0L)
                printf("success\n");
            else
                printf("failure\n");
            s->destroy(s);
            break;
          }
          case 14: {
            printf("Test addition of multiple, unique values ... ");
            const Set *s = create(dohash, doNothing, ncmp, 0L, 0.0, nhash);
            long values[] = {1, 2, 3, 4, 5};
            int i, success = 1;
            for (i = 0; i < 5; i++) {
                if (! s->add(s, (void *)values[i])) {
                    success = 0;
                    break;
                }
            }
            if (success && s->size(s) == 5L)
                printf("success\n");
            else
                printf("failure\n");
            s->destroy(s);
            break;
          }
          case 15: {
            printf("Test toArray() ... ");
            const Set *s = create(dohash, doNothing, ncmp, 0L, 0.0, nhash);
            long len, values[] = {1, 2, 3, 4, 5};
            int i, success = 0;
            long *array;
            for (i = 0; i < 5; i++)
                (void)s->add(s, (void *)values[i]);
            array = (long *)s->toArray(s, &len);
            if (array != NULL && len == 5L) {
                for (i = 0; i < 5; i++) {
                    if (findLong(array[i], values, 5L))
                        success++;
                }
             }
             if (success)
                printf("success\n");
            else
                printf("failure\n");
            free(array);
            s->destroy(s);
            break;
          }
          case 16: {
            printf("Test itCreate() ... ");
            const Set *s = create(dohash, doNothing, ncmp, 0L, 0.0, nhash);
            long values[] = {1, 2, 3, 4, 5};
            long v;
            int i, success = 0;
            const Iterator *it;
            for (i = 0; i < 5; i++)
                (void)s->add(s, (void *)values[i]);
            it = s->itCreate(s);
            if (it != NULL) {
                i = 0;
                while (it->hasNext(it)) {
                    (void)it->next(it, (void **)&v);
                    if (findLong(v, values, 5L))
                        success++;
                    i++;
                }
            }
            if (it != NULL && i == 5 && success)
                printf("success\n");
            else
                printf("failure\n");
            if (it != NULL)
                it->destroy(it);
            s->destroy(s);
            break;
          }
          case 17: {
            printf("Test sorting of elements ");
            const Set *s = create(dohash, doNothing, ncmp, 0L, 0.0, nhash);
            long len, values[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
            long i;
            long *array;
            for (i = 0; i < 10; i++)
                (void)s->add(s, (void *)values[i]);
            array = (long *)s->toArray(s, &len);
            sort((void **)array, len, ncmp);
            for (i = 0; i < len; i++)
                printf("%ld ", array[i]);
            printf("... success\n");
            free(array);
            s->destroy(s);
            break;
          }
          case 18: {
            printf("Test hashmap expansion ");
            const Set *s = create(dohash, doNothing, ncmp, 0L, 0.0, nhash);
            long len, values[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
                                    21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};
            long i;
            long *array;
            for(i = 0; i < 31; i++){
                (void)s->add(s, (void *)values[i]);
            }
            array = (long *)s->toArray(s, &len);
            sort((void **)array, len, ncmp);
            for(i = 0; i < len; i++){
                printf("%ld ", array[i]);
            }
            printf("... success\n");
            free(array);
            s->destroy(s);
            break;
          }
          default: {
            printf("Undefined test\n");
            break;
          }
       }
    }
}
