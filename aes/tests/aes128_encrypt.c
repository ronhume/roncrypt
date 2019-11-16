#include "common.h"
#include "aes.h"

#define IN_FILE "./testfile.dat"
#define OUT_FILE "./enc.aes128"

int main ()
{
    uint32_t salt[4] = { 0xFFFEFDFC,
                         0xFBFAF9F8,
                         0xF7F6F5F4,
                         0xF3F2F1F0 };

    uint32_t key[4] = { 0x01010101UL,
                        0x01010101UL,
                        0x01010101UL,
                        0x01010101UL };

    aes_file( IN_FILE,
              OUT_FILE,
              key,
              salt,
              KEY_128,
              ENCRYPT);
}
