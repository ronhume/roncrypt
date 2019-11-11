/*
 *
 * File: aes.h
 * Author: Ron Hume
 * Copyright (c) 2019, All rights reserved.
 *
 */

#ifndef _AES_H
#define _AES_H

typedef enum { ENCRYPT, DECRYPT } aes_mode_t;

typedef enum {
    KEY_128,
    KEY_192,
    KEY_256
} key_size_t;

void aes (uint32_t *input, 
          uint32_t *output,
          size_t    length,
          uint32_t  key[],
          uint32_t  salt[4],
          key_size_t keysize);

void aes_file( const char* const in,
               const char* const out,
               uint32_t key[],
               uint32_t salt[4],
               key_size_t keysize,
               aes_mode_t mode);

#endif // _AES_H
