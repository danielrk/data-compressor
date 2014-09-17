// Trie data structure
typedef struct node *Trie;

// Return an empty Trie
Trie createT (void);

// Destroy T; return NULL
Trie destroyT (Trie t);

// Return location of (w, K) in table if inside
// Otherwise NULL
Trie getT (Trie t, int K); 

// Return 1 if T encoded
// 0 otherwise
int hasCode (Trie t);

// Return #appearances in T
int getNapT(Trie t);

// Return code in T
int getCodeT (Trie t);

// Increment #appearances in T
void sawT (Trie t);

// Insert (w, K) into table
// Does not check if already exists
void insertT (Trie t, int K);  

// Update code in T
void encodeT (Trie t, int code);


