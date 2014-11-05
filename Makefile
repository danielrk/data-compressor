CC     = gcc
CFLAGS = -g3 -std=c99 -pedantic -Wall

encode: encode.o trie.o
	${CC} ${CFLAGS} -o encode encode.o trie.o code1.o
	ln -f encode decode

trie.o : trie.c trie.h
