#include <stdlib.h>
#include <stdio.h>
#include "trie.h"

#define DIE(msg) fprintf (stderr, "%s\n", msg), exit (EXIT_FAILURE)
#define DIE_FORMAT(format,value)  fprintf (stderr, format, value), exit (EXIT_FAILURE)

// Nodes will be placed in ARRAY for pruning
static Trie *arr = NULL;
static int   arr_size = 0;

// Trie data structure
struct node {
    int K;     // store char in int
    int code;  // code assigned  
    Trie *tv;  // array of subTries
    int tc;    // #elts in tv
    int nap;   // number of appearances
};


// Return a one-element trie with K set to C
// with code I, #appearances = NAP
static Trie makeNode (int c, int i, int nap)
{
    Trie t = malloc (sizeof(*t));
    t->K = c;
    t->code = i;
    t->tv = NULL;
    t->tc = 0;
    t->nap = nap;
    return t;
}

// Return a one-element trie whose contents are meaningless 
// EXCEPT tv, tc
Trie createT (void)
{
    Trie t = malloc (sizeof(*t));
    t->K = -1;
    t->code = -1;
    t->tv = NULL;
    t->tc = 0;
    t->nap = -1;
    
    // Initialize ARR at 1 since first code is at most index 1
    arr = malloc(sizeof(Trie));
    arr_size = 1;

    return t;
}

// Free recursively
static void freeT (Trie t) 
{
    if (t == NULL){
        ;
    }
    for (int i = 0; i < t->tc; i++) {
        freeT(*((t->tv)+i));
    }

    free (t->tv);
    free (t);
}

// Destroy trie T
void destroyT (Trie t)
{
    freeT(t); 

    // Free array
    free (arr);
}


// Return #appearances in T
int getNapT(Trie t) {
    return t->nap;
}

// Return code in T
int getCodeT (Trie t) {
    return t->code;
}

// Increment #appearances in T
void sawT (Trie t) {
    (t->nap)++;
}



// Return index of TV where trie with
// char C is if it exists, or where it should be inserted 
// if it doesn't
static int search (Trie *tv, int tc, int c)
{
    if (tc < 0)
        DIE("tc < 0 in search");
    else if (tc == 0)
        return 0;
    else { 
        int i = tc / 2;
        int piv_char = tv[i]->K;
        if (tc == 1) {
            if (c < piv_char)
                return 0;
            else if (c > piv_char)
                return 1;
            else if (c == piv_char) { // found
                return 0;
            }
        }
        else if (tc > 1) {
            if (c < piv_char)
                return search(tv, i, c);
            else if (c > piv_char) {
                int new_s = i + 1;
                return new_s + search(tv+new_s, tc-new_s, c);
            }
            else if (c == piv_char) { // found
                return i;
            }
        }
        return -1; // compiler
    }
}

// Return child of T with K and increment NAP if exists,
// Otherwise NULL
Trie getT (Trie t, int K) {
    int loc = search(t->tv, t->tc, K);
    if (loc >= t->tc) // not in trie
        return NULL;
    else { 
        Trie t_child = *(t->tv + loc);

        // K among t's children?
        if (t_child->K == K) {
            sawT(t_child); // increment NAP
            return t_child;
        }

        // string not in table
        return NULL;
    }
}




// Insert K as child of T with code I, NAP=nap
// Assumes K isn't already inserted
void insertT (Trie t, int K, int i, int nap)
{
    int loc = search(t->tv, t->tc, K); // where
    Trie *new_tv = malloc(sizeof(Trie)*(t->tc + 1));// could optimize time by doubling when out of space
    for (int m = 0; m < loc; m++) {
        new_tv[m] = *((t->tv)+m);
    }

    new_tv[loc] = makeNode(K, i, nap);
    // put node into array
    arr = realloc(arr, sizeof(Trie) * (++arr_size) );
    arr[i] = new_tv[loc];


    for (int m = loc + 1; m < t->tc + 1; m++) {
        new_tv[m] = *((t->tv)+m-1);
    }
    if (t->tc > 0) // free old TV if not empty
        free(t->tv);
    t->tv = new_tv;
    (t->tc)++;
}

void encodeT (Trie t, int i) {
    if (t == NULL)
        DIE("error: encoding a null trie");
    t->code = i;
}

// Print out the keys in the trie
void printT (Trie t, int level) {
    printf("Level %d: (code %d, key %d)\n", level, t->code, t->K);
    for (int i = 0; i < t->tc; i++) {
        printT(*((t->tv) + i), level + 1);
    }
}
