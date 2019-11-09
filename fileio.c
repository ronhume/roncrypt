/*
 *
 * File: fileio.c
 * Author: Ron Hume
 * Copyright (c) 2019, All rights Reserved.
 * 
 */

#include "common.h"
#include "fileio.h"

/* 
 * Convert char array to longlong little endian
 * Paramters: in, 
 *            out
 *
 * Return: void
 */
static void char_to_longlong( unsigned char* in, uint64_t *out )
{
    *out  = ((uint64_t)(*(in++)))<<56;
    *out |= ((uint64_t)(*(in++)))<<48;
    *out |= ((uint64_t)(*(in++)))<<40;
    *out |= ((uint64_t)(*(in++)))<<32;
    *out |= ((uint64_t)(*(in++)))<<24;
    *out |= ((uint64_t)(*(in++)))<<16;
    *out |= ((uint64_t)(*(in++)))<< 8;
    *out |= ((uint64_t)(*(in++)))    ;
}

/* 
 * Convert longlong little endian to char array
 * Paramters: in, 
 *            out
 *
 * Return: void
 */
static void longlong_to_char(uint64_t in, unsigned char *out) 
{
    *((out)++)=(unsigned char)(((in)>>56)&0xff);
    *((out)++)=(unsigned char)(((in)>>48)&0xff);
    *((out)++)=(unsigned char)(((in)>>40)&0xff);
    *((out)++)=(unsigned char)(((in)>>32)&0xff);
    *((out)++)=(unsigned char)(((in)>>24)&0xff);
    *((out)++)=(unsigned char)(((in)>>16)&0xff);
    *((out)++)=(unsigned char)(((in)>> 8)&0xff);
    *((out)++)=(unsigned char)(((in)    )&0xff);
}


/* 
 * Open file for reading
 * Parameters: filename
 *
 * Return: void
 */
int openinfile(const char* filename)
{
    return open(filename,O_RDONLY);
}

/* 
 * open file for writing
 * Parameters: filename
 *
 * Return: void
 */
int openoutfile(const char* filename)
{
    return open(filename, O_WRONLY| O_CREAT,S_IRUSR|S_IWUSR);
}

/* 
 * convert file descriptor to buffered to output
 * Parameters: fd - file descriptor
 *             bufsz - size of desired buffer
 *             vbuffer - pointer to buffer pointer
 *
 * Return: void
 */
FILE* outfile_buffered(int fd, size_t bufsz, char**vbuffer)
{
    FILE* fp = fdopen(fd, "w");
    if ( fp == NULL )
    {
        perror("fdopen()");
        exit(1);
    }

    /* allocate memory */
    *vbuffer = malloc (bufsz);
    if ( *vbuffer == NULL )
    {
        perror("malloc()");
        exit(1);
    }

    /* setvbuf */
    setvbuf(fp, *vbuffer, _IOFBF, bufsz);

    return fp;
}

/* 
 * Read DES-sized block of data from file, with proper 
 * PKCS #5/#7 padding on last block.
 * Parameters: fd - file descriptor
 *             block - pointer to block (out param)
 *
 * Return: void
 */
ssize_t des_readblock(int fd, uint64_t *block)
{
    unsigned char pad = 0;
    unsigned char buffer[DES_BLKSZ];

    ssize_t bytes = read(fd, (char*)buffer, DES_BLKSZ);

    if ( bytes <= 0 )
        return bytes;

    if ( bytes < DES_BLKSZ )
        pad = (DES_BLKSZ - bytes);

    /* PKCS #5/#7 padding */
    while ( bytes < DES_BLKSZ )
        buffer[bytes++]=pad;
    
    char_to_longlong ( buffer, block );
    
    return bytes;
}

/* 
 * Write DES block to file
 * Parameters: fd - file descriptor
 *             block - data block to be written
 *
 * Return: void
 */
void des_writeblock(int fd, uint64_t block)
{
    unsigned char buffer[DES_BLKSZ];

    longlong_to_char ( block, buffer );

    write(fd, (char*)buffer, DES_BLKSZ);
}

/* 
 * Write DES block to buffered file
 * Parameters: fd - file descriptor
 *             block - data block to be written
 *
 * Return: void
 */
void des_writeblock_buffered(FILE* fd, uint64_t block)
{
    unsigned char buffer[DES_BLKSZ];

    longlong_to_char ( block, buffer );

    fwrite(buffer, DES_BLKSZ, 1, fd);
}

/* 
 * Close file descriptor
 * Parameters: fd - file descriptor
 *
 * Return: void
 */
void closefile(int fd)
{
    close(fd);
}

/* 
 * Close buffered file descriptor
 * Parameters: fd - file descriptor
 *             vbuffer - pointer to buffer pointer
 *
 * Return: void
 */
void closefile_buffered(FILE* fd, void** vbuffer)
{
    fclose(fd);
    free(*vbuffer);
    *vbuffer = NULL;
}

