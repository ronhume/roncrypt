#include "../common.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "fileio.h"

static void char_to_longlong( unsigned char *c, uint64_t *ll )
{
    *ll  = ((uint64_t)(*(c++)))<<56LL;
    *ll |= ((uint64_t)(*(c++)))<<48LL;
    *ll |= ((uint64_t)(*(c++)))<<40LL;
    *ll |= ((uint64_t)(*(c++)))<<32LL;
    *ll |= ((uint64_t)(*(c++)))<<24LL;
    *ll |= ((uint64_t)(*(c++)))<<16LL;
    *ll |= ((uint64_t)(*(c++)))<< 8LL;
    *ll |= ((uint64_t)(*(c++)))<< 0LL;
}

static void longlong_to_char(uint64_t ll, unsigned char *c) 
{
    *((c)++)=(unsigned char)(((ll)>>56LL)&0xff);
    *((c)++)=(unsigned char)(((ll)>>48LL)&0xff);
    *((c)++)=(unsigned char)(((ll)>>40LL)&0xff);
    *((c)++)=(unsigned char)(((ll)>>32LL)&0xff);
    *((c)++)=(unsigned char)(((ll)>>24LL)&0xff);
    *((c)++)=(unsigned char)(((ll)>>16LL)&0xff);
    *((c)++)=(unsigned char)(((ll)>> 8LL)&0xff);
    *((c)++)=(unsigned char)(((ll)>> 0LL)&0xff);
}


int openinfile(const char* filename)
{
    return open(filename,O_RDONLY);
}

int openoutfile(const char* filename)
{
    return open(filename, O_WRONLY| O_CREAT,S_IRUSR|S_IWUSR);
}

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

void des_writeblock(int fd, uint64_t block)
{
    unsigned char buffer[DES_BLKSZ];

    longlong_to_char ( block, buffer );

    write(fd, (char*)buffer, DES_BLKSZ);
}

void closefile(int fd)
{
    close(fd);
}

