#include <stdio.h>
#include "fileio.h"

int main ( int argc, char* argv[] )
{
    char buffer[DES_BLKSZ];

    int fd =  openinfile("./foo");
    int fd2 = openoutfile("./foo2");
    while (readblock(fd,buffer)>0)
        writeblock(fd2,buffer);

    closefile(fd);
    closefile(fd2);
}

