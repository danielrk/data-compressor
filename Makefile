CC     = gcc
CFLAGS = -g3 -std=c99 -pedantic -Wall
HWK2   = /c/cs323/Hwk2/
encode: encode.o trie.o
	${CC} ${CFLAGS} -o encode encode.o trie.o ${HWK2}code.o
	ln -f encode decode

trie.o : trie.c trie.h
