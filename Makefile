.phony: all clean

all: des

OPTIONS = -Wall

des: testdes.c des.c des.h des_tables.h common.h
	gcc -o des $(OPTIONS) des.c testdes.c

clean: 
	rm -f des
