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
    int opt, dohash;

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
            const Set *s = create(dohash, free, scmp, 0L, 0.0, shash);
            if (s != NULL) {
                printf("success\n");
                s->destroy(s);
            } else
                printf("failure\n");
            break;
          }
          case 2: {
            printf("Test addition of a single value ... ");
            const Set *s = create(dohash, free, scmp, 0L, 0.0, shash);
            if (s->add(s, (void *)strdup("42")))
                printf("success\n");
            else
                printf("failure\n");
            s->destroy(s);
            break;
          }
          case 3: {
            printf("Test addition of a duplicate value ... ");
            const Set *s = create(dohash, free, scmp, 0L, 0.0, shash);
            char *sp = strdup("42");
            (void)s->add(s, (void *)sp);
            if (! s->add(s, (void *)sp))
                printf("success\n");
            else
                printf("failure\n");
            s->destroy(s);
            break;
          }
          case 4: {
            printf("Test isEmpty() on an empty set ... ");
            const Set *s = create(dohash, free, scmp, 0L, 0.0, shash);
            if (s->isEmpty(s))
                printf("success\n");
            else
                printf("failure\n");
            s->destroy(s);
            break;
          }
          case 5: {
            printf("Test isEmpty() on a non-empty set ... ");
            const Set *s = create(dohash, free, scmp, 0L, 0.0, shash);
            (void)s->add(s, (void *)strdup("42"));
            if (! s->isEmpty(s))
                printf("success\n");
            else
                printf("failure\n");
            s->destroy(s);
            break;
          }
          case 6: {
            printf("Test contains() on an empty set ... ");
            const Set *s = create(dohash, free, scmp, 0L, 0.0, shash);
            if (! s->contains(s, "42"))
                printf("success\n");
            else
                printf("failure\n");
            s->destroy(s);
            break;
          }
          case 7: {
            printf("Test contains() on a non-empty set and value is present ... ");
            const Set *s = create(dohash, free, scmp, 0L, 0.0, shash);
            (void)s->add(s, (void *)strdup("42"));
            if (s->contains(s, "42"))
                printf("success\n");
            else
                printf("failure\n");
            s->destroy(s);
            break;
          }
          case 8: {
            printf("Test contains() on a non-empty set and value is not present ... ");
            const Set *s = create(dohash, free, scmp, 0L, 0.0, shash);
            (void)s->add(s, (void *)strdup("42"));
            if (! s->contains(s, "99"))
                printf("success\n");
            else
                printf("failure\n");
            s->destroy(s);
            break;
          }
          case 9: {
            printf("Test remove() on an empty set ... ");
            const Set *s = create(dohash, free, scmp, 0L, 0.0, shash);
            if (! s->remove(s, "99"))
                printf("success\n");
            else
                printf("failure\n");
            s->destroy(s);
            break;
          }
          case 10: {
            printf("Test remove() on a non-empty set and value is present ... ");
            const Set *s = create(dohash, free, scmp, 0L, 0.0, shash);
            (void)s->add(s, (void *)strdup("42"));
            if (s->remove(s, "42"))
                printf("success\n");
            else
                printf("failure\n");
            s->destroy(s);
            break;
          }
          case 11: {
            printf("Test remove() on a non-empty set and value is not present ... ");
            const Set *s = create(dohash, free, scmp, 0L, 0.0, shash);
            (void)s->add(s, (void *)strdup("42"));
            if (! s->remove(s, "99"))
                printf("success\n");
            else
                printf("failure\n");
            s->destroy(s);
            break;
          }
          case 12: {
            printf("Test size() on an empty set ... ");
            const Set *s = create(dohash, free, scmp, 0L, 0.0, shash);
            if (s->size(s) == 0L)
                printf("success\n");
            else
                printf("failure\n");
            s->destroy(s);
            break;
          }
          case 13: {
            printf("Test size() on a non-empty set ... ");
            const Set *s = create(dohash, free, scmp, 0L, 0.0, shash);
            (void)s->add(s, (void *)strdup("42"));
            if (s->size(s) != 0L)
                printf("success\n");
            else
                printf("failure\n");
            s->destroy(s);
            break;
          }
          case 14: {
            printf("Test addition of multiple, unique values ... ");
            const Set *s = create(dohash, free, scmp, 0L, 0.0, shash);
            char *values[] = {"1", "2", "3", "4", "5"};
            int i, success = 1;
            for (i = 0; i < 5; i++) {
                if (! s->add(s, (void *)strdup(values[i]))) {
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
            const Set *s = create(dohash, free, scmp, 0L, 0.0, shash);
            char *values[] = {"1", "2", "3", "4", "5"};
            long len;
            int i, success = 0;
            char **array;
            for (i = 0; i < 5; i++)
                (void)s->add(s, (void *)strdup(values[i]));
            array = (char **)s->toArray(s, &len);
            if (array != NULL && len == 5L) {
                for (i = 0; i < 5; i++) {
                    if (findStr(array[i], values, 5L))
                        success++;
                }
            }
            if (array != NULL && len == 5L && success)
                printf("success\n");
            else
                printf("failure\n");
            free(array);
            s->destroy(s);
            break;
          }
          case 16: {
            printf("Test itCreate() ... ");
            const Set *s = create(dohash, free, scmp, 0L, 0.0, shash);
            char *values[] = {"1", "2", "3", "4", "5"};
            char *sp;
            int i, success = 0;
            const Iterator *it;
            for (i = 0; i < 5; i++)
                (void)s->add(s, (void *)strdup(values[i]));
            it = s->itCreate(s);
            if (it != NULL) {
                i = 0;
                while (it->hasNext(it)) {
                    (void)it->next(it, (void **)&sp);
                    if (findStr(sp, values, 5L))
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
            const Set *s = create(dohash, free, scmp, 0L, 0.0, shash);
            long len, i;
            char *values[] = {" 1", " 2", " 3", " 4", " 5", " 6",
                                  " 7", " 8", " 9", "10" };
            char **array;
            for (i = 0; i < 10; i++)
                (void)s->add(s, (void *)strdup(values[i]));
            array = (char **)s->toArray(s, &len);
            sort((void **)array, len, scmp);
            for (i = 0; i < len; i++)
                printf("%s ", array[i]);
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
