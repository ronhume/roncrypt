#ifndef __FILEIO_H
#define __FILEIO_H

#define DES_BLKSZ 4   /* 32 bits */
#define DES_PAD_CHAR 0xFF

int openinfile(const char* filename);
int openoutfile(const char* filename);
ssize_t readblock(int fd, char*buffer);
void writeblock(int fd, const char*buffer);
void closefile(int fd);

#endif //__FILEIO_H
