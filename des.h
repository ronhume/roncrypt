/*
 *
 * File: des.h
 * Author: Ron Hume
 * Copyright (c) 2019, All rights Reserved.
 * 
 */

#ifndef _DES_H
#define _DES_H

typedef enum { ENCRYPT, DECRYPT } des_mode_t;

void des( uint64_t *input, 
          uint64_t *output,
          size_t  length,
          uint64_t key,
          uint64_t salt,
          des_mode_t mode );

void des_file( const char* const in, 
               const char* const out,
               uint64_t key,
               uint64_t salt,
               des_mode_t mode );

void tripledes( uint64_t *input, 
                uint64_t *output,
                size_t  length,
                uint64_t key[],
                uint64_t salt,
                des_mode_t mode );

void tripledes_file( const char* const in,
                     const char* const out,
                     uint64_t key[],
                     uint64_t salt,
                     des_mode_t mode );

#endif //_DES_H
