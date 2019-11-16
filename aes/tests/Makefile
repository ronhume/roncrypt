.phony: all clean

TARGETS= aes128_encrypt aes256_encrypt aes128_decrypt aes256_decrypt aes_mem

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

aes128_decrypt: aes128_decrypt.c
	gcc -o aes128_decrypt $(OPTIONS) $(INCDIRS) $(LIBDIR) $(AESLIB) aes128_decrypt.c

aes256_decrypt: aes256_decrypt.c
	gcc -o aes256_decrypt $(OPTIONS) $(INCDIRS) $(LIBDIR) $(AESLIB) aes256_decrypt.c

clean: 
	rm -f $(TARGETS)
