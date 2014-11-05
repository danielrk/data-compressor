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
 
void prune(Trie t) {
    ;
}
int encode(int MAXBITS, int E_FLAG, int P_FLAG) {
    
    int next_code = 0; // == number of codes assigned
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
    while ((int K = getchar()) != EOF) {
        child = getT(C, K);
        if (child != NULL)
            C = child;
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
                if (C == t)
                    insertT(C, K, next_code++, 0); // nap++ below on getT
                else
                    insertT(C, K, next_code++, 1);
            }
            else if (C == t) // failed insert new single-char
                continue;
            
            // =========== UPDATE NBITS =======================

            // Prune as soon as last slot taken
            if (P_FLAG && next_code == (int)pow(2, MAXBITS))
                prune();

            // Increase NBITS only when #codes assigned
            // exceeds it
            else if (next_code > (int)pow(2, nBits))
                nBits++;

            C = getT(t, K); // increments NAP

            if (C == NULL) { // (EMPTY, K) not in table
                if (!E_FLAG)
                    DIE_FORMAT("E_FLAG false, yet (EMPTY, K=%d) not in table\n", K);

                ungetc(K, stdin); // single-char on next insert
                C = t;
            }
    }
    printT(t, 0);
    destroyT(t);

    return 0;
}

int decode() {
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

        encode(MAXBITS, E_FLAG, P_FLAG);
    }
    else {
        if (argc > 1)
            DIE("usage: decode");

        decode();
    }

    return 0;
}
