/*
 *
 * File: des_decrypt.c
 * Author: Ron Hume
 * Copyright (c) 2019, All rights Reserved.
 * 
 */

#include "common.h"
#include "des.h"
#include "fileio.h"

int main()
{
    //uint64_t key = 0x70617373776F7264;
    uint64_t key =   0x0102030405060708;
    //uint64_t salt = 0x696E697469616C7A;
    uint64_t salt =   0xFFFEFDFCFBFAF9F8;

    int infile = openinfile("./enc.des");
    int outfile = openoutfile("./dec.des");

    des_file(infile, outfile, key, salt, DECRYPT);

    closefile(infile);
    closefile(outfile);
}

