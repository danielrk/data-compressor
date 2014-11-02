#include <stdlib.h>

#define DIE(msg) fprintf (stderr, "%s\n", msg), exit (EXIT_FAILURE)
#define DIE_FORMAT(format,value)  fprintf (stderr, format, value), exit (EXIT_FAILURE)

// Trie data structure
struct node {
    int K;     // store char in int
    int code;  // code assigned  
    Trie *tv;  // array of subTries
    int tc;    // #elts in tv
    int nap;   // number of appearances
};


// Return a one-element trie with K set to C
// with code I
static Trie makeNode (int c, int i)
{
    Trie t = malloc (sizeof(*t));
    t->K = c;
    t->code = i;
    t->tv = NULL;
    t->tc = 0;
    t->nap = 0;
    return t;
}

// Return a one-element trie whose contents are meaningless 
// EXCEPT tv, tc
Trie createT (void)
{
    Trie t = malloc (sizeof(*t));
    t->K = '\0';
    t->code = -1;
    t->tv = NULL;
    t->tc = 0;
    t->nap = -1;
    return t;
}

// Destroy trie T
void destroyT (Trie t)
{
    if (t == NULL){
        ;
    }
    for (int i = 0; i < t->tc; i++) {
        destroyT(*((t->tv)+i));
    }
    free (t->tv);
    free (t);
}

// Insert (w, K) into table with code I
// Assumes prefix is in trie and last node is not
void insertT (Trie t, int *Kv, int Kc, int i)
{
    if (Kc < 1)
        DIE("insertT with Kc<1");
    if (Kc == 1) { // 
        int c   = *Kv;                      // char to insert
        int loc = bsearch(t->tv, t->tc, c); // where
        Trie *new_tv = malloc(sizeof(Trie)*(t->tc + 1));// could optimize time by doubling when out of space
        for (int m = 0; m < loc; m++) {
            new_tv[m] = *((t->tv)+m);
        }
        new_tv[loc] = makeNode(c, i);
        for (int m = loc + 1; m < t->tc + 1; m++) {
            new_tv[m] = *((t->tv)+m-1);
        }
        if (tc > 0) // free old TV if not empty
            free(t->tv);
        t->tv = new_tv;
    }
    else { // not at leaf
        int loc = bsearch(t->tv, t->tc, c);
        insertT(((t->tv)+loc)*, Kv+1, Kc-1, i);
    }
}


// Return index of TV where trie with
// char C is if it exists, or where it should be inserted 
// if it doesn't
static int bsearch (Trie *tv, int tc, int c)
{
    if (tc < 0)
        DIE("tc < 0 in bsearch");
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
                return bsearch(tv, i, c);
            else if (c > piv_char) {
                new_s = i + 1;
                return new_s + bsearch(tv+new_s, tc-new_s, c);
            }
            else if (c == piv_char) { // found
                return i;
            }
        }
    }
}

void encodeT (Trie t, int i) {
    if (t == NULL)
        DIE("error: encoding a null trie")
    t->code = i;
}


