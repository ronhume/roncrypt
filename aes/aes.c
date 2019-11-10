/*
 *
 * File: aes.c
 * Author: Ron Hume
 * Copyright (c) 2019, All rights Reserved.
 * 
 */

#include "common.h"
#include "aes_tables.h"

#define WORD_MASK 0x00000000FFFFFFFFULL;

typedef enum {
    KEY_128,
    KEY_192,
    KEY_256
} key_size_t;

static void rol8 ( uint32_t *data )
{
    register uint64_t tmp_data = (*data);
    uint8_t tmp = tmp_data>>24;

    tmp_data = (tmp_data<<40)>>32;
    tmp_data += tmp;
    *data = (uint32_t)tmp_data;
}

static void substitute ( uint32_t *data )
{
    *data = aes_sbox[ ((*data)     >>24)]<<24 |
            aes_sbox[(((*data)<< 8)>>24)]<<16 |
            aes_sbox[(((*data)<<16)>>24)]<< 8 |
            aes_sbox[(((*data)<<24)>>24)]     ;
}

static void aes_key_schedule ( uint32_t key[],
                               uint32_t subkeys[],
                               key_size_t keysize )
{
    uint8_t round_constant = 0x01; 
    uint32_t upper_byte;
    size_t num_keys, num_subkeys;

    switch (keysize)
    {
    case KEY_128:
        num_keys = 4;
        break;
    case KEY_192:
        num_keys = 6;
        break;
    case KEY_256:
        num_keys = 8;
        break;
    }
 
    num_subkeys = (num_keys+7)<<2;

    memcpy (subkeys, key, (num_keys<<2));

    for ( size_t i = num_keys; i < num_subkeys; i++ )
    {
        subkeys[i] = subkeys[i-1];
        
        if ( !( i % num_keys ) )
        {
            rol8       (&(subkeys[i]));
            substitute (&(subkeys[i]));
        
            /* reset round constant */
            if ( !(i % 36) )
                round_constant = 0x1B;

            upper_byte = round_constant;
            upper_byte <<= 24;
            subkeys[i] ^= upper_byte;
            round_constant <<= 1;
        }
        else if ( ( keysize == KEY_256 ) && ( i % num_keys ) == 4)
        {
            substitute (&(subkeys[i]));
        }

        subkeys[i] ^= subkeys[i-num_keys];
    }
}

int main ()
{
    int i;

    uint32_t key[3][8] = { 
                        { 0x01010101UL,
                          0x01010101UL,
                          0x01010101UL,
                          0x01010101UL },
                        { 0x01010101UL,
                          0x01010101UL,
                          0x01010101UL,
                          0x01010101UL,
                          0x01010101UL,
                          0x01010101UL },
                        { 0x01010101UL,
                          0x01010101UL,
                          0x01010101UL,
                          0x01010101UL,
                          0x01010101UL,
                          0x01010101UL,
                          0x01010101UL,
                          0x01010101UL }
                     };

    uint32_t subkeys[60];

    for ( i = 0; i < 60; i++ )
        subkeys[i] = 0;

    aes_key_schedule(key[0], subkeys, KEY_128);
    for ( i = 0; i < 60; i++ )
        printf ( "[%d]: 0x%08X\n", i, subkeys[i]);
#if 0
    for ( i = 0; i < 60; i++ )
        subkeys[i] = 0;

    aes_key_schedule(key[1], subkeys, KEY_192);
    for ( i = 0; i < 60; i++ )
        printf ( "[%d]: 0x%08X\n", i, subkeys[i]);

    for ( i = 0; i < 60; i++ )
        subkeys[i] = 0;

    aes_key_schedule(key[2], subkeys, KEY_256);
    for ( i = 0; i < 60; i++ )
        printf ( "[%d]: 0x%08X\n", i, subkeys[i]);
#endif
}
