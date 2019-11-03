# Triple DES implemention
## DES and 3DES Library
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
ssize_t des_readblock(int fd, uint64_t *block)
```
Reads in a DES-sized block for processing, including proper PKCS #5/#7 padding.
```C
void des_writeblock(int fd, uint64_t block)
```
Writes out a DES-sized block.
```C
void closefile(int fd)
```
Closes the open file.

#### DES and 3DES APIs

Mode is either __ENCRYPT__ or __DECRYPT__

```C
void des( uint64_t *input, 
          uint64_t *output,
          size_t  length,
          uint64_t key,
          uint64_t salt,
          des_mode_t mode );
```
Performs DES operation on an input memory buffer of length __length__, and puts result in output buffer, using key and salt for encryption.  Salt allows support of CBC mode.
```C
void des_file( int in_fd, 
               int out_fd,
               uint64_t key,
               uint64_t salt,
               des_mode_t mode );
```
Performs DES operation on an input file, and puts result in output file, using key and salt for encryption.  Salt allows support of CBC mode.
```C
void tripledes( uint64_t *input, 
                uint64_t *output,
                size_t  length,
                uint64_t key[],
                uint64_t salt,
                des_mode_t mode );
```
Performs 3DES operation on an input memory buffer of length __length__, and puts result in output buffer, using key and salt for encryption.  Salt allows support of CBC mode.
```C
void tripledes_file( int in_fd,
                     int out_fd,
                     uint64_t key[],
                     uint64_t salt,
                     des_mode_t mode );
```
Performs 3DES operation on an input file, and puts result in output file, using key and salt for encryption.  Salt allows support of CBC mode.

### Test Routines
* __des_encrypt__
* __des_decrypt__
* __tripledes_encrypt__
* __tripledes_decrypt__
### OpenSSL Interoperability
Use procedures outlines in __tests/testscript.txt__ to demonstrate interoperability with OpenSSL.
