#include "common.h"
#include "aes.h"

#define IN_FILE "./testfile.dat"
#define OUT_FILE128 "./enc.aes128"
#define OUT_FILE256 "./enc.aes256"

int main ()
{
    uint32_t salt[2][4] = { { 0xFFFEFDFC,
                              0xFBFAF9F8,
                              0xF7F6F5F4,
                              0xF3F2F1F0 },
                            { 0xFFFEFDFC,
                              0xFBFAF9F8,
                              0xF7F6F5F4,
                              0xF3F2F1F0 }
                          };

    uint32_t key[2][8] = { 
                        { 0x01010101UL,
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
              salt[0],
              KEY_128,
              ENCRYPT);

    aes_file( IN_FILE,
              OUT_FILE256,
              key[1],
              salt[1],
              KEY_256,
              ENCRYPT);
}
