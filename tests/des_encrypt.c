/*
 *
 * File: des_encrypt.c
 * Author: Ron Hume
 * Copyright (c) 2019, All rights Reserved.
 * 
 */

#include "common.h"
#include "des.h"
#include "fileio.h"

int main()
{
    uint64_t key = 0x70617373776F7264;
    uint64_t salt = 0x696E697469616C7A;

    int infile = openinfile("./testfile.dat");
    int outfile = openoutfile("./enc.des");

    des_file(infile, outfile, key, salt, ENCRYPT);

    closefile(infile);
    closefile(outfile);
}

