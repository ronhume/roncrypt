.phony: all clean

all: libdes.a

OPTIONS = -Wall
DES_DEPS = des.c des.h des_tables.h common.h
DES_TOOLS_DEPS = fileio.c fileio.h
INCDIRS=-I. 

libdes.a: des.o fileio.o
	ar rcs libdes.a des.o fileio.o

des.o: $(DES_DEPS)
	gcc -c $(OPTIONS) des.c $(INCDIRS)

fileio.o: $(DES_TOOLS_DEPS)
	gcc -c $(OPTIONS) $(INCDIRS) fileio.c

clean: 
	rm -f libdes.a fileio.o des.o
