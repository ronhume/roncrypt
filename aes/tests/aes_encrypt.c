#include "common.h"
#include "aes.h"

#define IN_FILE "./testfile.dat"
#define OUT_FILE128 "./enc.aes128"
#define OUT_FILE192 "./enc.aes192"
#define OUT_FILE256 "./enc.aes256"

int main ()
{
    uint32_t salt[4] = {
                          0xFFFEFDFC,
                          0xFBFAF9F8,
                          0xF7F6F5F4,
                          0xF3F2F1F0
                       };

    uint32_t key[3][8] = { 
                        { 0x01010101UL,
                          0x01010101UL,
                          0x01010101UL,
                          0x01010101UL },
                        { 0x01010101UL,
                          0x01010101UL,
                          0x01010101UL,
                          0x01010101UL,
                          0x01010101UL,
                          0x01010101UL },
                        { 0x01010101UL,
                          0x01010101UL,
                          0x01010101UL,
                          0x01010101UL,
                          0x01010101UL,
                          0x01010101UL,
                          0x01010101UL,
                          0x01010101UL }
                     };

    aes_file( IN_FILE,
              OUT_FILE128,
              key[0],
              salt,
              KEY_128,
              ENCRYPT);

    aes_file( IN_FILE,
              OUT_FILE192,
              key[1],
              salt,
              KEY_192,
              ENCRYPT);

    aes_file( IN_FILE,
              OUT_FILE256,
              key[2],
              salt,
              KEY_256,
              ENCRYPT);
}
