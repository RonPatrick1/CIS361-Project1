CC = gcc
CFLAGS = -c -Wall -std=c99

all: encrypt decrypt

encrypt: encrypt.o
	$(CC) encrypt.o -o encrypt

encrypt.o: encrypt.c enc_dec.h
	$(CC) $(CFLAGS) encrypt.c

decrypt: decrypt.o
	$(CC) decrypt.o -o decrypt
	
decrypt.o: decrypt.c enc_dec.h khash.h
	$(CC) $(CFLAGS) decrypt.c

.PHONY : clean
clean:
	rm encrypt.o decrypt.o encrypt decrypt
