/*
 *
 * File: tripledes_encrypt.c
 * Author: Ron Hume
 * Copyright (c) 2019, All rights Reserved.
 * 
 */

#include "common.h"
#include "des.h"
#include "fileio.h"

int main()
{
    //uint64_t salt = 0x696E697469616C7A;
    uint64_t salt =   0xFFFEFDFCFBFAF9F8;

#if 0
    uint64_t key_3des[3] = {0x7477656E7479666F,
                            0x7572636861726163,
                            0x746572696E707574};
#else
    uint64_t key_3des[3] = {0x0102030405060708,
                            0x090A0B0C0D0E0F10,
                            0x1112131415161718};
#endif

    int infile = openinfile("./testfile.dat");
    int outfile = openoutfile("./enc.3des");

    tripledes_file(infile, outfile, key_3des, salt, ENCRYPT);

    closefile(infile);
    closefile(outfile);
}

