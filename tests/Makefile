.phony: all clean

TARGETS= destest_mem \
         des_encrypt \
         des_decrypt \
         tripledes_encrypt \
         tripledes_decrypt

all: $(TARGETS)

OPTIONS = -Wall -O3
INCDIRS=-I. -I..
LIBDIR=-L..
DESLIB=-ldes

destest_mem: 
	gcc -o destest_mem $(OPTIONS) $(INCDIRS) $(LIBDIR) $(DESLIB) destest_mem.c

des_encrypt: 
	gcc -o des_encrypt $(OPTIONS) $(INCDIRS) $(LIBDIR) $(DESLIB) des_encrypt.c

des_decrypt: 
	gcc -o des_decrypt $(OPTIONS) $(INCDIRS) $(LIBDIR) $(DESLIB) des_decrypt.c

tripledes_encrypt: 
	gcc -o tripledes_encrypt $(OPTIONS) $(INCDIRS) $(LIBDIR) $(DESLIB) tripledes_encrypt.c

tripledes_decrypt: 
	gcc -o tripledes_decrypt $(OPTIONS) $(INCDIRS) $(LIBDIR) $(DESLIB) tripledes_decrypt.c

clean: 
	rm -f $(TARGETS)
