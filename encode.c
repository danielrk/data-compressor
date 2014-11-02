#include "trie.h"

int main (int argc, char **argv)
{
    Trie t = createT();
    int a1[] = {1};
    int a2[] = {2};
    int a3[] = {3};
    int a4[] = {1, 2};
    int a5[] = {1, 3};
    int a6[] = {1, 10};

    int code = 1;
    insertT(t, a1, 1, code++);
    insertT(t, a2, 1, code++);
    insertT(t, a3, 1, code++);
    insertT(t, a4, 2, code++);
    insertT(t, a5, 2, code++);
    insertT(t, a6, 2, code++);

    destroyT(t);

    return 0;
}
