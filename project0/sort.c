#include "sort.h"
#include <stdlib.h>

static void **scratch = NULL;

static void merge(void *a[], long L, long M, long H, int (*cmp)(void*,void*)) {
    long l1 = L, l2 = M + 1, i;

    for (i = L; l1 <= M && l2 <= H; i++) {
        if((*cmp)(a[l1], a[l2]) <= 0)
            scratch[i] = a[l1++];
        else
            scratch[i] = a[l2++];
    }
    while (l1 <= M)
        scratch[i++] = a[l1++];
    while (l2 <= H)
        scratch[i++] = a[l2++];
    for (i = L; i <= H; i++)
        a[i] = scratch[i];
}

static void msort(void *a[], long low, long high, int(*cmp)(void*, void*)) {
    if (low < high) {
        long mid = (low + high) / 2;
        msort(a, low, mid, cmp);
        msort(a, mid+1, high, cmp);
        merge(a, low, mid, high, cmp);
    }
}

void sort(void *a[], long size, int (*cmp)(void *v1, void *v2)) {
    scratch = (void **)malloc(size*sizeof(void *));
    msort(a, 0L, size-1, cmp);
    free(scratch);
}
