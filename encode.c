#include "trie.h"
#include "/c/cs323/Hwk2/code.h" 
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>

#define DIE(msg) fprintf (stderr, "%s\n", msg), exit (EXIT_FAILURE)
#define DIE_FORMAT(format,value)  fprintf (stderr, format, value), exit (EXIT_FAILURE)

// Special codes
int ESCAPE = 0;

// Return smallest #bits necessary to
// write NCODES codes
int get_nbits(int nCodes) {

    int n = 1;
    int max = (int)pow(2,n);
    while (max < nCodes)
        max = (int)pow(2,++n);
    
    return n;
}



// Compress STDIN into stream of codes
int encode(int MAXBITS, int E_FLAG, int P_FLAG) {
    
    // Send option args encoded as:
    // MAXBITS: 6 bits (since max value is 20)
    // E_FLAG: 1 bit
    // P_FLAG: 1 bit
    
    putBits(6, MAXBITS);
    putBits(1, E_FLAG);
    putBits(1, P_FLAG);


    
    int next_code = 0; // == number of codes assigned == # elts in ARRAY
    int nBits = 1;     // #bits required to send NEXT code
    
    if (E_FLAG)
        next_code = 1; // already assigned 0 to ESCAPE

    Trie t = createT();

    if (!E_FLAG) { // initialize all one-char strings
        for (int K = 0; K < 256; K++)
            insertT(t, K, next_code++, 0);

        nBits = 8;
    }

    Trie C = t;
    int K;
    while ((K = getchar()) != EOF) {
        Trie child = getT(C, K);
        if (child != NULL) { // increment NAP and go down trie
            sawT(child); 
            C = child;
        }
        else { 
            // ============ PUTBITS ==========================
            if (C == t) { // new 1-char string
                if (!E_FLAG)
                    DIE_FORMAT("E_FLAG false, yet (EMPTY, K=%d) not in table\n", K);

                putBits(nBits, ESCAPE);
                putBits(CHAR_BIT, K); 
            }
            else {
                // Output code C
                putBits(nBits, getCodeT(C));
            } 

            
            // =========== INSERT ==============================

            // insert new code if table not full
            if (next_code < (int)pow(2, MAXBITS)) {

                insertT(C, K, next_code++, 1);
            }
            

            // =========== UPDATE NBITS =======================

            // Prune as soon as last slot taken
            if (next_code == (int)pow(2, MAXBITS)) {
                if (P_FLAG) {

                    next_code = prune(&t, E_FLAG);
                    nBits = get_nbits(next_code);
                }
                else
                    ;
            }

            // Increase NBITS only when #codes assigned
            // exceeds it
            else if (next_code > (int)pow(2, nBits))
                nBits++;
            
            // ============ RESET C =====
            if (C == t)         // new single-char, so skip
                continue;
            else {
                C = getT(t, K);

                if (C == NULL) { // (EMPTY, K) not in table
                    if (!E_FLAG)
                        DIE_FORMAT("E_FLAG false, yet (EMPTY, K=%d) not in table\n", K);

                    ungetc(K, stdin); // single-char on next insert
                    C = t;
                }
                else
                    sawT(C);     // increment NAP
            }
        }
    }

    if (C != t)
        putBits(nBits, getCodeT(C));
    
    flushBits();

    //printT(t, 0);
    destroyT(t); 
    return 0;
}

// Print string associated with CODE to stdout,
// incrementing NAP for each node visited
// Return the left-most char (finalK) in string
// Assumes CODE != EMPTY
int putstring(int code) {

    Trie t = C_to_T(code);
    sawT(t);
    if (pref(code) == EMPTY) {
        putchar(t->K); 
        return t->K;
    }
    else {
        int finalK = putstring(pref(code));
        putchar(t->K);
        return finalK;
    }
}

// Decompress stream of bits from encode
int decode() {

    int MAXBITS = getBits(6);
    int E_FLAG  = getBits(1);
    int P_FLAG  = getBits(1);

    int EMPTY = -1; // code for empty string
    int STANDBY = -2; // dummy K inserted to be replaced later

    int next_code = 0; // == number of codes assigned == # elts in ARRAY
    int nBits = 1;     // #bits required to send NEXT code
    
    if (E_FLAG)
        next_code = 1; // already assigned 0 to ESCAPE

    Trie t = createT();

    if (!E_FLAG) { // initialize all one-char strings
        for (int K = 0; K < 256; K++)
            insertT(t, K, next_code++, 0);

        nBits = 8;
    }

    int oldC = EMPTY;
    int newC;
    int C;
    int KwK = 0; // KwK flag

    while ((newC = getBits(nBits)) != EOF) {
        C = newC;
        
        C_to_T(C)  
        
        // Print string associated with C
        int finalK = putstring(C);

        if (KwK)
            putchar(finalK);
        
        // Insert node with C as prefix and K=STANDBY
        insertT( C_to_T(C), STANDBY, next_code++, 1);

        // Prune (if so,  Update C, oldC, newC )
        // Update nBits

        // K now known for word inserted with prefix OLDC
        updateK(oldC, finalK);
        

        // Update OLDC
        oldC = newC;
        KwK = 0;
    }
    return 0;
}


int main (int argc, char **argv)
{
    if (strcmp(argv[0]+strlen(argv[0])-6, "encode") == 0) {
        int MAXBITS = 12;
        int E_FLAG  = 0;
        int P_FLAG  = 0;

        int m_flag = 0; // previous arg was -m
        for (int i = 1; i < argc; i++) {
            if (m_flag == 1) {
                int m_spec = atoi(argv[i]);
                if (m_spec <= 0)
                    DIE("nonpositive MAXBITS");
                
                if (m_spec <= 8 || m_spec > 20)
                    ;
                else
                    MAXBITS = m_spec;

                m_flag = 0;
            }
            else if (strcmp(argv[i], "-m") == 0)
                m_flag = 1;
            else if (strcmp(argv[i], "-e") == 0)
                E_FLAG = 1;
            else if (strcmp(argv[i], "-p") == 0)
                P_FLAG = 1;
            else
                DIE("usage: encode [-m MAXBITS] [-p] [-e]");
        }
        if (m_flag) // last arg was "-m"
            DIE("usage: encode [-m MAXBITS] [-p] [-e]");

        // Trie t = createT();

        // int code = 1;
        // insertT(t, 1, code++, 1);
        // insertT(t, 2, code++, 1);
        // insertT(t, 3, code++, 5);
        // 
        // prune(&t, 1);

        // printT(t, 0);
        // destroyT(t);

        encode(MAXBITS, E_FLAG, P_FLAG);
    }
    else {
        if (argc > 1)
            DIE("usage: decode");

        decode();
    }

    return 0;
}
