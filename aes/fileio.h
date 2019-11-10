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

#define AES_BLKSZ 16   /* 128 bits */

int openinfile(const char* filename);
int openoutfile(const char* filename);

FILE* outfile_buffered(int fd, size_t bufsz, char**vbuffer);

ssize_t aes_readblock(int fd, uint32_t block[4]);

void aes_writeblock(int fd, uint32_t block[4]);
void aes_writeblock_buffered(FILE* fd, uint32_t block[4]);

void closefile(int fd);
void closefile_buffered(FILE* fd, void** vbuffer);

#endif //__FILEIO_H
