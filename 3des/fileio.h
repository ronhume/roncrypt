/*
 *
 * File: fileio.h
 * Author: Ron Hume
 * Copyright (c) 2019, All rights Reserved.
 * 
 */

#ifndef __FILEIO_H
#define __FILEIO_H
#include "common.h"

#define DES_BLKSZ 8   /* 64 bits */
#define DES_PAD_CHAR 0xFF

int openinfile(const char* filename);
int openoutfile(const char* filename);

FILE* outfile_buffered(int fd, size_t bufsz, char**vbuffer);

ssize_t des_readblock(int fd, uint64_t *block);

void des_writeblock(int fd, uint64_t block);
void des_writeblock_buffered(FILE* fd, uint64_t block);

void closefile(int fd);
void closefile_buffered(FILE* fd, void** vbuffer);

#endif //__FILEIO_H
