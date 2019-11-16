#include "common.h"
#include "aes.h"

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

    uint32_t data[8] = { 
                        0x01020304,
                        0x05060708,
                        0x090A0B0C,
                        0x0D0E0F10,
                        0x01020304,
                        0x05060708,
                        0x090A0B0C,
                        0x0D0E0F10 };

    uint32_t output[8] = { 0,0,0,0,0,0,0,0 };
    uint32_t output2[8] = { 0,0,0,0,0,0,0,0 };
    
    aes( data, output, 8, key[0], salt[0], KEY_128, ENCRYPT );

    for ( int i = 0; i < 8; i++ )
    {
        printf ( "OUT[%i]: 0x%08X\n", i, output[i] );
    }

    aes( output, output2, 8, key[0], salt[1], KEY_128, DECRYPT );

    for ( int i = 0; i < 8; i++ )
    {
        printf ( "OUT[%i]: 0x%08X\n", i, output2[i] );
    }
}
