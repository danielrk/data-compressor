// Trie data structure
typedef struct node *Trie;

// Return an empty Trie
Trie createT (void);

// Destroy T; return NULL
void destroyT (Trie t);

// Return location of (w, K) in table if inside
// Otherwise NULL
Trie getT (Trie t, int *Kv, int Kc); 

// Return #appearances in T
int getNapT(Trie t);

// Return code in T
int getCodeT (Trie t);

// Increment #appearances in T
void sawT (Trie t);

// Insert (w, K) into trie
// Assumes prefix is in trie
void insertT (Trie t, int *Kv, int Kc, int i);  

// Update code in T to I
void encodeT (Trie t, int i);


