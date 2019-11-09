/*
 *
 * File: des_decrypt.c
 * Author: Ron Hume
 * Copyright (c) 2019, All rights Reserved.
 * 
 */

#include "common.h"
#include "des.h"

#define IN_FILE "./enc.des"
#define OUT_FILE "./dec.des"

int main()
{
    uint64_t key =   0x0102030405060708;
    uint64_t salt =   0xFFFEFDFCFBFAF9F8;

    des_file(IN_FILE, OUT_FILE, key, salt, DECRYPT);
}

