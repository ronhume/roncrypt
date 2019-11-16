.phony: all clean

TARGETS= aes128_encrypt aes256_encrypt aes_decrypt aes_mem

all: $(TARGETS)

OPTIONS = -Wall -O3
INCDIRS=-I. -I..
LIBDIR=-L..
AESLIB=-laes

aes_mem: aes_mem.c
	gcc -o aes_mem $(OPTIONS) $(INCDIRS) $(LIBDIR) $(AESLIB) aes_mem.c

aes128_encrypt: aes128_encrypt.c
	gcc -o aes128_encrypt $(OPTIONS) $(INCDIRS) $(LIBDIR) $(AESLIB) aes128_encrypt.c

aes256_encrypt: aes256_encrypt.c
	gcc -o aes256_encrypt $(OPTIONS) $(INCDIRS) $(LIBDIR) $(AESLIB) aes256_encrypt.c

aes_decrypt: aes_decrypt.c
	gcc -o aes_decrypt $(OPTIONS) $(INCDIRS) $(LIBDIR) $(AESLIB) aes_decrypt.c

clean: 
	rm -f $(TARGETS)
