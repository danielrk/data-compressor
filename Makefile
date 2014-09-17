CC     = gcc
CFLAGS = -g3 -std=c99 -pedantic -Wall

encode: encode.o
	${CC} ${CFLAGS} -o encode encode.o
	ln -f encode decode
