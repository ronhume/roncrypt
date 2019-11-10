.phony: all clean

all: libaes.a

OPTIONS = -Wall -O3
AES_DEPS = aes.c aes.h aes_tables.h common.h
AES_TOOLS_DEPS = fileio.c fileio.h
INCDIRS=-I. 

libaes.a: aes.o fileio.o
	ar rcs libaes.a aes.o fileio.o

aes.o: $(AES_DEPS)
	gcc -c $(OPTIONS) aes.c $(INCDIRS)

fileio.o: $(AES_TOOLS_DEPS)
	gcc -c $(OPTIONS) $(INCDIRS) fileio.c

clean: 
	rm -f libaes.a fileio.o aes.o
