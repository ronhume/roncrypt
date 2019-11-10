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
    uint64_t key = 0x0102030405060708;
    uint64_t salt = 0xFFFEFDFCFBFAF9F8;
    uint64_t key_3des[3] = {0x0102030405060708,
                            0x090A0B0C0D0E0F10,
                            0x1112131415161718};

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

