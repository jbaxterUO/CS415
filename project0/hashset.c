#include "hashset.h"  /* the .h file does NOT reside in /usr/local/include/ADTs */
#include <stdlib.h>
#include <ADTs/ADTdefs.h>
#include <ADTs/iterator.h>
/* any other includes needed by your code */
#define MAX_SET_CAPACITY 134217728L //taken from the CaDS21F book for reference
#define TRIGGER_LIMIT 100L;
#define UNUSED __attribute__((unused))

typedef struct mentry {
    void * value;
} MEntry;

typedef struct node {
    struct node *next;
    MEntry entry;
} Node;

typedef struct s_data {
    /* definitions of the data members of self */
    long size;
    long capacity;
    long changes;
    double load;
    double loadFactor;
    double increment;
    Node** table;
    void (*freeValue)(void *v);
    long (*hashFxn)(void *m, long N);
    int (*cmpFxn)(void*, void*);


} SData;

/*
 * important - remove UNUSED attributed in signatures when you flesh out the
 * methods
 */

static void purge(const SData *sd) {
    long i;

    for(i = 0L; i < sd->capacity; i++)
    {
        Node *current, *next;
        current = sd->table[i];

        while(current != NULL)
        {
            sd->freeValue((current->entry).value);
            next = current->next;
            free(current);
            current = next;
        }

        sd->table[i] = NULL;
    }
}

static void s_destroy(const Set *s){
    SData *sd = (SData *)s->self;
    purge(sd);
    free(sd->table);
    free(sd);
    free((void *)s);
}

static void s_clear(const Set *s) {
    SData *sd = (SData *)s->self;
    purge(sd);
    sd->size = 0L;
    sd->load = 0;
    sd->changes = 0.0;

}


static void resize(SData *sd){
    long N;
    Node *current, *next, **array;

    long i, j;

    N = 2 * sd->capacity;
    
    if(N > MAX_SET_CAPACITY){
        N = MAX_SET_CAPACITY;
    }

    if(N == sd->capacity){
        return;
    }

    array = (Node **)malloc(N * sizeof(Node *));

    if(array == NULL){
        return;
    }

    for(j = 0; j < N; j++){
        array[j] = NULL;
    }

    for(i = 0; i < sd->capacity; i++){
        for(current = sd->table[i]; current != NULL; current = next){
            next = current->next;
            j = sd->hashFxn(current->entry.value, N);
            current->next = array[j];
            array[j] = current;
        }
    }

    free(sd->table);
    sd->table = array;
    sd->capacity = N;
    sd->load /= 2.0;
    sd->changes = 0;
    sd->increment = 1.0 / (double)N;


}

static bool insertEntry(SData *sd, void *value, long i){
    Node *current = (Node *)malloc(sizeof(Node));
    bool status;
    status = (current != NULL);

    if(status){
        (current->entry).value = value;
        current->next = sd->table[i];
        sd->table[i] = current;
        sd->size++;
        sd->load += sd->increment;
        sd->changes ++;
    } else {
        free(current);
        status = false;
    }

    return status;
}

static bool s_add(const Set *s, void *member) {
    SData *sd = (SData *)s->self;
    long i;
    Node *c;
    bool status = false;

    if(sd->load > sd->loadFactor){
        resize(sd);
    }

    i = sd->hashFxn(member, sd->capacity);
    c = sd->table[i];

    if(c == NULL){
        status = insertEntry(sd, member, i);
    }


    return status;
}

static bool s_contains(const Set *s, void *member) {
    SData *sd = (SData *)s->self;
    long i = sd->hashFxn(member, sd->capacity); //get index in table to start at
    bool isFound = false;
    Node* cur;
    for(cur = sd->table[i]; cur != NULL; cur = cur->next) //traverse potential linked list
    {
        if(sd->cmpFxn(member, cur->entry.value) == 0)
        {
            isFound = true;
            break;
        }
    }

    return isFound;
}

static bool s_isEmpty(const Set *s) {
    SData *sd = (SData *)s->self;
    return sd->size == 0;
}

static bool s_remove(const Set *s, void *member) {
    SData *sd = (SData *)s->self;
    long i = sd->hashFxn(member, sd->capacity);
    Node *entry = sd->table[i];
    bool status;
    status = (entry != NULL);

    if(status){
        Node *c, *n;
        for(c = NULL, n = sd->table[i]; n != entry; c = n, n = c->next){
            if(c == NULL){
                sd->table[i] = entry->next;
            }
            else{
                c->next = entry->next;
                sd->size--;
                sd->load -= sd->increment;
                sd->changes++;
                sd->freeValue((entry->entry).value);
                free(entry);
            }
        }
    }

    return status;
}

static long s_size(const Set *s) {
    SData *sd = (SData *)s->self;
    return sd->size;
}

static void **entries(SData *sd){
    void **tmp;
    tmp = NULL;

    if(sd->size > 0L) {
        tmp = (void **)malloc(sizeof(void *) * sd->size);
        if(tmp != NULL){
            long i, n = 0L;
            for(i = 0L; i < sd->capacity; i++){
                Node *c = sd->table[i];
                while(c != NULL){
                    tmp[n++] = (c->entry).value;
                    c = c->next;
                }
            }
        }
    }

    return tmp;
}


static void **s_toArray(const Set *s, long *len) {
    SData *sd = (SData *)s->self;
    void **tmp = entries(sd);

    if(tmp != NULL){
        *len = sd->size;
    }

    return tmp;
}

static const Iterator *s_itCreate(const Set *s) {
    SData *sd = (SData *)s->self;
    const Iterator *it = NULL;
    void **tmp = entries(sd);

    if(tmp != NULL) {
        it = Iterator_create(sd->size, tmp);
        if(it == NULL){
                free(tmp);
        }
    }
    
    return it;
}


    


static Set template = {
    NULL, s_destroy, s_clear, s_add, s_contains, s_isEmpty, s_remove,
    s_size, s_toArray, s_itCreate
};

const Set *HashSet(void (*freeValue)(void*), int (*cmpFxn)(void*, void*),
                    long capacity, double loadFactor,
                   long (*hashFxn)(void *m, long N)
                  ) {
    
    Set *s = (Set *)malloc(sizeof(Set));
    long c;
    double lf;
    Node **array;

    long i;

    if(s != NULL){
        SData *sd = (SData *)malloc(sizeof(SData));

        if(sd != NULL){
            c = ((capacity > 0) ? capacity : DEFAULT_SET_CAPACITY);
            if(c > MAX_SET_CAPACITY){
                c = MAX_SET_CAPACITY;
            }

            lf = ((loadFactor > 0.000001) ? loadFactor : DEFAULT_LOAD_FACTOR);
            array = (Node **)malloc(c * sizeof(Node *));

            if(array != NULL)
            {
                sd->capacity = c;
                sd->loadFactor = lf;
                sd->size = 0L;
                sd->load = 0.0;
                sd->changes = 0L;
                sd->increment = 1.0 / (double)c;
                sd->freeValue = freeValue;
                sd->table = array;
                for(i = 0; i < c; i++)
                {
                    array[i] = NULL;
                }
                sd->cmpFxn = cmpFxn;
                sd->hashFxn = hashFxn;

                *s = template;
                s->self = sd;

            } else {
                free(sd);
                free(s);
                s = NULL;
            }
        } else {
            free(s);
            s = NULL;
        }
    }

    return s;
}
