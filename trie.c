
// Trie data structure
struct node {
    int K;     // store char in int
    int code;  // code assigned  
    int assigned; // flag
    Trie *tv;  // array of subTries
    int tc;    // #elts in tv
    int nap;   // number of appearances
};


// Return a one-element trie with K set to C
static Trie makeNode (int c)
{
    Trie t = malloc (sizeof(*t));
    t->K = c;
    t->code = -1;
    t->assigned = 0;
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
    t->assigned = -1;
    t->tv = NULL;
    t->tc = 0;
    t->nap = -1;
    return t;
}

// Destroy trie T; return NULL
Trie destroyT (Trie t)
{
    if (t == NULL)
        return NULL;
    for (int i = 0; i < t->tc; i++) {
        destroyT(*((t->tv)+i));
    }
    free (t->tv);
    free (t);
    return NULL;
}

// Make new trie w/ char C as a child of T; 
// return the child
Trie insertT (Trie t, int c)
{
    if (t->tc == 0 && t->tv == NULL) {
        t->tv = malloc(sizeof(Trie));
        *(t->tv) = makeNode(c);
        return *(t->tv);
    }
    else {
        t->tv = realloc(t->tv, sizeof(Trie)*(t->tc + 1));
   //     *((t->tv)+(t->tc)) = makeNode(c); bin search
        t->tc += 1;
    }
