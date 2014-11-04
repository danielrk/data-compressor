// Trie data structure
typedef struct node *Trie;

// Return an empty Trie
Trie createT (void);

// Destroy T; return NULL
void destroyT (Trie t);

// Return child of T with K if exists,
// Otherwise NULL
Trie getT (Trie t, int K);

// Return #appearances in T
int getNapT(Trie t);

// Return code in T
int getCodeT (Trie t);

// Increment #appearances in T
void sawT (Trie t);

// Insert K as child of T with code I
// Assumes K isn't already inserted
void insertT (Trie t, int K, int i);  

// Update code in T to I
void encodeT (Trie t, int i);

// Print out the keys in the trie
void printT (Trie t, int level);


