// Trie data structure
typedef struct node *Trie;

// Return an empty Trie
Trie createT (void);

// Destroy T; return NULL
void destroyT (Trie t);

// Return child of T with K and increment NAP if exists,
// Otherwise NULL
Trie getT (Trie t, int K);

// Return #appearances in T
int getNapT(Trie t);

// Return code in T
int getCodeT (Trie t);

// Increment #appearances in T
void sawT (Trie t);

// Insert K as child of T with code I, NAP=nap
// Assumes K isn't already inserted
void insertT (Trie t, int K, int i, int nap);  

// Delete nodes with low NAP
// and reassign codes
// Return new # codes assigned
int prune (Trie *pT, int E_FLAG);

// Update code in T to I
void encodeT (Trie t, int i);

// Print out the keys in the trie
void printT (Trie t, int level);

// Return trie associated with CODE
Trie C_to_T (int code);

// Return K associated with CODE
// Assumes CODE already assigned
int getK (int code);

// Return code of the prefix of string
// associated with CODE
// or -1 if prefix empty
// Assumes CODE is already assigned
int pref (int code);

// Node with code CODE gets char K
void updateK (int code, int K);
