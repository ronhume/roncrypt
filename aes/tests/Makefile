.phony: all clean

TARGETS= aes_encrypt aes_decrypt aes_mem

all: $(TARGETS)

OPTIONS = -Wall -O3
INCDIRS=-I. -I..
LIBDIR=-L..
AESLIB=-laes

aes_mem: aes_mem.c
	gcc -o aes_mem $(OPTIONS) $(INCDIRS) $(LIBDIR) $(AESLIB) aes_mem.c

aes_encrypt: aes_encrypt.c
	gcc -o aes_encrypt $(OPTIONS) $(INCDIRS) $(LIBDIR) $(AESLIB) aes_encrypt.c

aes_decrypt: aes_decrypt.c
	gcc -o aes_decrypt $(OPTIONS) $(INCDIRS) $(LIBDIR) $(AESLIB) aes_decrypt.c

clean: 
	rm -f $(TARGETS)
