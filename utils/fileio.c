#include "common.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "fileio.h"


int openinfile(const char* filename)
{
    return open(filename,O_RDONLY);
}

int openoutfile(const char* filename)
{
    return open(filename, O_WRONLY| O_CREAT,S_IRUSR|S_IWUSR);
}

ssize_t readblock(int fd, char*buffer)
{
    ssize_t bytes = read(fd, buffer, DES_BLKSZ);

    if ( bytes <= 0 )
        return bytes;

    while ( bytes < DES_BLKSZ )
        buffer[bytes++]=DES_PAD_CHAR;
    
    return bytes;
}

void writeblock(int fd, const char*buffer)
{
    write(fd, buffer, DES_BLKSZ);
}

void closefile(int fd)
{
    close(fd);
}

