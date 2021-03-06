// encode, decode
//      File compression using the Lempel-Ziv-Welch algorithm.
//      By: Daniel Kim, 11/9/14
//      code.h provided by Stan Eisenstat
//
// usage: encode [-m MAXBITS] [-p] [-e]
//        decode


#include "trie.h"
#include "/c/cs323/Hwk2/code.h" 
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#define DIE(msg) fprintf (stderr, "%s\n", msg), exit (EXIT_FAILURE)
#define DIE_FORMAT(format,value)  fprintf (stderr, format, value), exit (EXIT_FAILURE)

// Special codes
int QUIT = 0;   // decode quits when reading trailing 0's in flushBits()
int ESCAPE = 1; // code sent before new 1-char (-e)
int EMPTY = -1; // code for empty string

// Special K
int STANDBY = -2; // decode: dummy K inserted to be replaced later




// Return smallest #bits necessary to
// write NCODES codes
int get_nbits(int nCodes) {

    int max = 2;
    int n   = 1;
    while (max < nCodes) {
        max = max << 1;
        n++;
    }
    
    return n;
}



// Compress STDIN into stream of codes
// First byte sent in the form [MAXBITS (6 bits)][E_FLAG][P_FLAG]
//
// The only special code sent is ESCAPE for -e;
// everything else is derived in decode.
// 
// Pruning performed as soon as the table is full
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
        next_code = 2; // already assigned 0 to QUIT
                       //                  1 to ESCAPE

    // ============== INITIALIZE TRIE ================
    Trie t = createT();

    if (!E_FLAG) { // initialize all one-char strings
        for (int K = 0; K < 256; K++)
            insertT(t, K, next_code++, 0);

        nBits = 8;
    }



    // ================ ENCODE INPUT =================

    Trie C = t;               // last node visited
    int K;
    while ((K = getchar()) != EOF) {


        Trie child = getT(C, K);

        if (child != NULL) {  // increment NAP and go down trie
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
            if (next_code < (1 << MAXBITS)) {

                insertT(C, K, next_code++, 1);
            }
            

            // =========== UPDATE NBITS =======================


            // Prune as soon as last slot taken
            if (next_code == (1 << MAXBITS)) {

                if (P_FLAG) {

                    next_code = prune(&t, E_FLAG);
                    nBits = get_nbits(next_code);
                }
                else
                    ;
            }

            // Increase NBITS only when #codes assigned
            // exceeds it
            else if (next_code > (1 << nBits))
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
    
    // Put leftover known prefix
    if (C != t) {
        putBits(nBits, getCodeT(C));
    }
    
    flushBits();

    destroyT(t); 
    return 0;
}




// Print string associated with CODE to stdout,
// incrementing NAP for each node visited
// Return 1 if entire string printed,
// 0 if KwK (Kw printed; not second K)
// Assumes CODE != EMPTY

int putstring(int code, int *pKwK) {

    Trie t = C_to_T(code); // DIEs if code invalid
    sawT(t);
    int K = getK(code);

    if (pref(code) == EMPTY) {

        putchar(K); 
        return K;
    }
    else {

        int finalK = putstring(pref(code), NULL);
        if (K != STANDBY) 
            putchar(K);

        else   // KwK; will put second K later
            *pKwK = 1; 

        return finalK;
    }
}



// Decompress stream of bits from encode
// using patching algorithm
//
// Exit if bitstream contains invalid codes

int decode() {

    // Decode first byte as options
    int MAXBITS = getBits(6);
    int E_FLAG  = getBits(1);
    int P_FLAG  = getBits(1);

    if (MAXBITS <= CHAR_BIT || MAXBITS > 20
           || E_FLAG == EOF || P_FLAG == EOF)
        DIE("decode: bit stream not encoded by encode");



    int next_code = 0; // == number of codes assigned == # elts in ARRAY
    int nBits = 1;     // #bits required to send NEXT code
    
    if (E_FLAG)
        next_code = 2; // already assigned 0 to QUIT
                       //                  1 to ESCAPE
    
    // =============== INITIALIZE TRIE =====================

    Trie t = createT();

    if (!E_FLAG) { // initialize all one-char strings
        for (int K = 0; K < 256; K++)
            insertT(t, K, next_code++, 0);

        nBits = 8;
    }
    


    // =============== DECODE BIT STREAM ====================

    int C;
    int last_insert = EMPTY; // code assigned to last inserted node
    while ((C = getBits(nBits)) != EOF) {
        
        // -e: Break on C = QUIT (flushBits() junk)
        if (E_FLAG && C == QUIT)
            break;



        // ========== PRINT STRING WITH NEW CODE =======

        int finalK; // first char in C string

        // -e: check for ESCAPE
        if (E_FLAG && C == ESCAPE) {
            finalK = getBits(CHAR_BIT);

            if (finalK == EOF)
                DIE("decode: bit stream not encoded by encode");

            putchar(finalK);
        }

        else {

            int KwK = 0;
            finalK = putstring(C, &KwK); // DIEs if C not in table

            // If C was just inserted w/ STANDBY (KwK), 
            // print oldC==Kw then K

            if (KwK)
                putchar(finalK);
        }
       

        // =========== PATCH LAST-INSERTED STRING =========

        // K now known for word inserted with prefix OLDC
        if (last_insert != EMPTY) 
            updateK(last_insert, finalK);

        
        
        // =========== INSERT NEW CODE ====================

        // insert new code if table not full
        if (next_code < (1 << MAXBITS)) {
            
            
            if (E_FLAG && (C == ESCAPE)) {
                insertT(t, finalK, next_code++, 1);
                last_insert = EMPTY;
            }
            else {

                // Insert node with C as prefix and K=STANDBY
                insertT( C_to_T(C), STANDBY, next_code, 1); 
                last_insert = next_code++;
            }
        }
        else
            last_insert = EMPTY; // no insert to update next time
        

        // =========== UPDATE NBITS =======================

        // Prune as soon as last slot taken
        if (next_code == (1 << MAXBITS)) {

            if (P_FLAG) {

                next_code = prune(&t, E_FLAG);
                nBits = get_nbits(next_code);
                // no need to update K in insertion
                // since it'll be pruned
                last_insert = EMPTY;
                
            }
            else
                ;
        }

        // Increase NBITS only when #codes assigned
        // exceeds it
        else if (next_code > (1 << nBits))
            nBits++;
            
    }

    destroyT(t);
    return 0;
}


int main (int argc, char **argv)
{
    // ============ ENCODE =====================

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

        encode(MAXBITS, E_FLAG, P_FLAG);
    }



    // ============== DECODE ==========================
    else {
        if (argc > 1)
            DIE("usage: decode");

        decode();
    }

    return 0;
}
