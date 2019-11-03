/*
 *
 * File: destest_mem.c
 * Author: Ron Hume
 * Copyright (c) 2019, All rights Reserved.
 * 
 */

#include "common.h"
#include "des.h"

int main()
{
    uint64_t key = 0x70617373776F7264;
    uint64_t key_3des[3] = {0x7477656E7479666F,
                            0x7572636861726163,
                            0x746572696E707574};
    uint64_t salt = 0x696E697469616C7A;
    uint64_t input[5] = {
            0x6162636465666768,
            0x6162636465666768,
            0x6162636465666768,
            0x6162636465666768,
            0x6162636465666768};

    uint64_t output[5];
    uint64_t output2[5];

    for ( int i = 0; i < 5; i++)
        printf ( "INPUT: 0x%" PRIx64 "\n", input[i]);

    des (input,output,5,key,salt,ENCRYPT);
    for ( int i = 0; i < 5; i++)
        printf ( "DES: 0x%" PRIx64 "\n", output[i]);

    des (output,output2,5,key,salt,DECRYPT);
    for ( int i = 0; i < 5; i++)
        printf ( "PT: 0x%" PRIx64 "\n", output2[i]);

    tripledes (input,output,5,key_3des,salt,ENCRYPT);
    for ( int i = 0; i < 5; i++)
        printf ( "3DES: 0x%" PRIx64 "\n", output[i]);

    tripledes (output,output2,5,key_3des,salt,DECRYPT);
    for ( int i = 0; i < 5; i++)
        printf ( "PT: 0x%" PRIx64 "\n", output2[i]);
}

