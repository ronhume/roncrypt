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

#define IN_FILE "./testfile.dat"
#define OUT_FILE "./enc.3des"

int main()
{
    uint64_t salt =   0xFFFEFDFCFBFAF9F8;

    uint64_t key_3des[3] = {0x0102030405060708,
                            0x090A0B0C0D0E0F10,
                            0x1112131415161718};

    tripledes_file(IN_FILE, OUT_FILE, key_3des, salt, ENCRYPT);
}

