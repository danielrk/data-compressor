#include <stdlib.h>
#include <stdio.h>
#include "trie.h"

#define DIE(msg) fprintf (stderr, "%s\n", msg), exit (EXIT_FAILURE)
#define DIE_FORMAT(format,value)  fprintf (stderr, format, value), exit (EXIT_FAILURE)

// Nodes will be placed in ARRAY for pruning
// ARR_SIZE === # codes assigned
// ARR[0] will be NULL w/ -e option; hold a node otherwise
static Trie *arr = NULL;
static int   arr_size = 0;

// Trie data structure
struct node {
    int K;     // store char in int
    int code;  // code assigned  
    Trie *tv;  // array of subTries
    int tc;    // #elts in tv
    int nap;   // number of appearances
    Trie s;    // parent node
};


// Return a one-element trie with K set to C
// with code I, #appearances = NAP, parent = S
static Trie makeNode (int c, int i, int nap, Trie s)
{
    Trie t = malloc (sizeof(*t));
    t->K = c;
    t->code = i;
    t->tv = NULL;
    t->tc = 0;
    t->nap = nap;
    t->s   = s;
    return t;
}

// Return a one-element empty trie 
// EXCEPT tv, tc
Trie createT (void)
{
    Trie t = malloc (sizeof(*t));
    t->K = -1;
    t->code = -1;
    t->tv = NULL;
    t->tc = 0;
    t->nap = -1;
    t->s = NULL;
    
    // Initialize ARR at 1 since first code is at most index 1
    arr = malloc(sizeof(Trie));
    arr[0] = NULL;
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

// Return child of T with K
// Otherwise NULL
Trie getT (Trie t, int K) {
    int loc = search(t->tv, t->tc, K);
    if (loc >= t->tc) // not in trie
        return NULL;
    else { 
        Trie t_child = *(t->tv + loc);

        // K among t's children?
        if (t_child->K == K) {
            //sawT(t_child); // increment NAP
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

    new_tv[loc] = makeNode(K, i, nap, t);
    // put node into array
    arr = realloc(arr, sizeof(Trie) * (i+1) );
    arr_size = i+1;
    arr[i] = new_tv[loc];


    for (int m = loc + 1; m < t->tc + 1; m++) {
        new_tv[m] = *((t->tv)+m-1);
    }
    if (t->tc > 0) // free old TV if not empty
        free(t->tv);
    t->tv = new_tv;
    (t->tc)++;
}

// Make TV contiguous (fill in NULL spots)
// and update TC
static void shrink (Trie t)
{
    Trie *new_tv = malloc(sizeof(Trie));
    int new_tc  = 0; // index for next NEW_TV insert

    for (int i = 0; i < t->tc; i++) {

        Trie child = *( (t->tv) + i);
        if (child != NULL) { // move non-NULL children
            new_tv = realloc(new_tv, sizeof(Trie) * (new_tc+1));
            new_tv[new_tc] = child;
            new_tc++;
        }
    }
    
    if ( t->tc > 0)
        free( t->tv); // free old TV

    t->tv = new_tv;   // set TV, TC
    t->tc = new_tc;

    if (new_tc == 0) { // new_tv is empty but malloc'ed 
        free( t->tv );
        t->tv = NULL;
    }
}

// Delete (and NULLify in parent TV) 
// nodes with low NAP and reassign codes
// Return # codes assigned
int prune (Trie *pT, int E_FLAG) 
{
    if ((*pT)->nap == -1) { // root

        // prune children
        for (int i = 0; i < (*pT)->tc; i++) {

            prune((*pT)->tv + i, E_FLAG);
        }
        shrink(*pT);


        // new ARR for code reassignment
        Trie *new_arr = malloc(sizeof(Trie));
        int new_i     = 0; // === # elts in NEW_ARR

        if (E_FLAG) { // code 0 already used
            *new_arr = NULL;
            new_i = 1;
        }
       
        // move to new array
        for (int j = 0; j < arr_size; j++) {

            if (arr[j] != NULL) {
                new_arr = realloc(new_arr, sizeof(Trie)*(new_i+1));
                new_arr[new_i] = arr[j];

                // new code = index in new array
                (new_arr[new_i])->code = new_i;

                new_i++;
            }
        }

        free(arr);
        arr = new_arr;
        arr_size = new_i;


        // Re-insert 1-char strings by default if necessary
        if (!E_FLAG) {
            for (int c = 0; c < 256; c++) {

                if (getT(*pT, c) == NULL)
                    insertT(*pT, c, arr_size++, 0);

            }
        }

        return arr_size; // == next_code == # codes assigned
    }
   
    // non-root node
    else {

        // Halve NAP
        int new_nap = (*pT)->nap / 2;
        (*pT)->nap = new_nap;

        // Prune children
        for (int i = 0; i < (*pT)->tc; i++) 
            prune((*pT)->tv + i, E_FLAG);
        
        // Update TV, TC
        shrink(*pT);

        // if nap/2 == 0, free + set spot in ARR,parent TV to NULL
        if (new_nap == 0) { 
            arr[(*pT)->code] = NULL;
            free((*pT)->tv); // children will have been freed
            free((*pT));
            *pT = NULL; // nullify spot in parent's TV
        }

        return 0;
    }
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


// Return code of the prefix of string
// associated with CODE
// or -1 if prefix empty
// Assumes CODE is already assigned
int pref (int code) {

    Trie parent = arr[code]->s;
    pref_code = parent->code;
    return pref_code;
}
