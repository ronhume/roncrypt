# AES implemention
## AES Library
### API Reference
#### FileIO APIs
```C
int openinfile ( const char* filename )
```
Opens an input file for reading.
```C
int openoutfile ( const char* filename )
```
Opens an output file for writing.
```C
FILE* outfile_buffered ( int fd, size_t bufsz, char** vbuffer)
```
Upgrades an int fd to FILE* and adds setvbuf() buffering.
```C
ssize_t aes_readblock(int fd, uint32_t block[4])
```
Reads in a AES-sized block for processing, including proper PKCS #5/#7 padding.
```C
void aes_writeblock(int fd, uint32_t blocki[4])
```
Writes out a AES-sized block.
```C
void aes_writeblock_buffered(FILE* fd, uint32_t blocki[4])
```
Writes out a AES-sized block to buffered I/O stream.
```C
void closefile(int fd)
```
Closes the open file.
```C
void closefile_buffered(FILE* fd, void **vbuffer)
```
Closes the open file that has been set up for buffered I/O, and frees the associated buffer.

#### AES APIs

Mode is either __ENCRYPT__ or __DECRYPT__.
Keysize is either __KEY_128__ or __KEY_256__.

```C
void aes( uint32_t *input, 
          uint32_t *output,
          size_t  length,
          uint32_t key[],
          uint32_t salt[4],
          key_size_t keysize
          aes_mode_t mode );
```
Performs AES operation on an input memory buffer of length __length__, and puts result in output buffer, using key and salt for encryption.  Salt allows support of CBC mode.
```C
void aes_file( const char* const in,
               const char* const out,
               uint32_t key[],
               uint32_t salt[4],
               key_size_t keysize,
               aes_mode_t mode );
```
Performs AES operation on an input file, and puts result in output file, using key and salt for encryption.  Salt allows support of CBC mode.

### Test Routines
* __aes_encrypt__
* __aes_decrypt__
### OpenSSL Interoperability
Use procedures outlined in __tests/testscript.txt__ to demonstrate interoperability with OpenSSL. Or, use __testit.sh__ to demonstrate performance and compatibility.
