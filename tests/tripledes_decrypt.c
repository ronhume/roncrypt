/*
 *
 * File: tripledes_decrypt.c
 * Author: Ron Hume
 * Copyright (c) 2019, All rights Reserved.
 * 
 */

#include "common.h"
#include "des.h"
#include "fileio.h"

int main()
{
    uint64_t key_3des[3] = {0x7477656E7479666F,
                            0x7572636861726163,
                            0x746572696E707574};
    uint64_t salt = 0x696E697469616C7A;

    int infile = openinfile("./enc.3des");
    int outfile = openoutfile("./dec.3des");

    tripledes_file(infile, outfile, key_3des, salt, DECRYPT);

    closefile(infile);
    closefile(outfile);
}

