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
 * Convert char array to long
 * Paramters: in, 
 *            out
 *
 * Return: void
 */
static void char_to_long( unsigned char* in, uint32_t *out )
{
    *out  = ((uint64_t)(*(in++)))<<24;
    *out |= ((uint64_t)(*(in++)))<<16;
    *out |= ((uint64_t)(*(in++)))<< 8;
    *out |= ((uint64_t)(*(in++)))    ;
}

/* 
 * Convert long to char array
 * Paramters: in, 
 *            out
 *
 * Return: void
 */
static void long_to_char(uint32_t in, unsigned char *out) 
{
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
 * Read AES-sized block of data from file, with proper 
 * PKCS #5/#7 padding on last block.
 * Parameters: fd - file descriptor
 *             block - pointer to block (out param)
 *
 * Return: void
 */
ssize_t aes_readblock(int fd, uint32_t block[])
{
    unsigned char pad = 0;
    unsigned char buffer[AES_BLKSZ];

    ssize_t bytes = read(fd, (char*)buffer, AES_BLKSZ);

    if ( bytes <= 0 )
        return bytes;

    if ( bytes < AES_BLKSZ )
        pad = (AES_BLKSZ - bytes);

    /* PKCS #5/#7 padding */
    while ( bytes < AES_BLKSZ )
        buffer[bytes++]=pad;
    
    char_to_long ( buffer   , &block[0] );
    char_to_long ( buffer+ 4, &block[1] );
    char_to_long ( buffer+ 8, &block[2] );
    char_to_long ( buffer+12, &block[3] );
    
    return bytes;
}

/* 
 * Write AES block to file
 * Parameters: fd - file descriptor
 *             block - data block to be written
 *
 * Return: void
 */
void aes_writeblock(int fd, uint32_t block[4])
{
    unsigned char buffer[AES_BLKSZ];

    long_to_char ( block[0], buffer   );
    long_to_char ( block[1], buffer+ 4);
    long_to_char ( block[2], buffer+ 8);
    long_to_char ( block[3], buffer+12);

    write(fd, (char*)buffer, AES_BLKSZ);
}

/* 
 * Write AES block to buffered file
 * Parameters: fd - file descriptor
 *             block - data block to be written
 *
 * Return: void
 */
void aes_writeblock_buffered(FILE* fd, uint32_t block[4])
{
    unsigned char buffer[AES_BLKSZ];

    long_to_char ( block[0], buffer   );
    long_to_char ( block[1], buffer+ 4);
    long_to_char ( block[2], buffer+ 8);
    long_to_char ( block[3], buffer+12);

    fwrite(buffer, AES_BLKSZ, 1, fd);
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

