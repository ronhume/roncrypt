.phony: all clean

all: des

OPTIONS = -Wall

des: des.c des_tables.h common.h
	gcc -o des $(OPTIONS) des.c

clean: 
	rm -f des
