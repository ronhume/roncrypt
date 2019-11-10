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

static uint32_t get_key_words ( key_size_t size )
{
    uint32_t num_keys = 0;

    switch (size)
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
 
    return num_keys;
}

static void print_array(const char* const string, uint32_t array[4])
{
    for(int i = 0; i<4; i++)
    {
        printf("%s[%d]: 0x%08X\n", string, i, array[i]);
    }
}

static void print_byte_array(
    const char* const string,
    unsigned char array[4][4])
{
  for (int i=0;i<4;i++)
  {
      printf("%s[%d]: 0x%02X%02X%02X%02X\n",
          string,
          i,
          array[i][0],
          array[i][1],
          array[i][2],
          array[i][3]);
  }
}

static void rol8 ( uint32_t *data )
{
    register uint64_t tmp_data = (*data);
    uint8_t tmp = tmp_data>>24;

    tmp_data = (tmp_data<<40)>>32;
    tmp_data += tmp;
    *data = (uint32_t)tmp_data;
}

static void transpose ( uint32_t input[4], uint32_t output[4] )
{
    output[0] =  (input[0] & 0xFF000000UL)      ;
    output[0] |= (input[1] & 0xFF000000UL) >>  8;
    output[0] |= (input[2] & 0xFF000000UL) >> 16;
    output[0] |= (input[3] & 0xFF000000UL) >> 24;

    output[1] =  (input[0] & 0x00FF0000UL) <<  8;
    output[1] |= (input[1] & 0x00FF0000UL)      ;
    output[1] |= (input[2] & 0x00FF0000UL) >>  8;
    output[1] |= (input[3] & 0x00FF0000UL) >> 16;

    output[2] =  (input[0] & 0x0000FF00UL) << 16;
    output[2] |= (input[1] & 0x0000FF00UL) <<  8;
    output[2] |= (input[2] & 0x0000FF00UL)      ;
    output[2] |= (input[3] & 0x0000FF00UL) >>  8;

    output[3] =  (input[0] & 0x000000FFUL) << 24;
    output[3] |= (input[1] & 0x000000FFUL) << 16;
    output[3] |= (input[2] & 0x000000FFUL) <<  8;
    output[3] |= (input[3] & 0x000000FFUL)      ;
}

static void make_byte_array ( uint32_t input[4], uint8_t output[4][4] )
{
    for ( uint8_t i = 0; i < 4; i++ )
    {
        output[i][0] = (input[i] & 0xFF000000)>>24;
        output[i][1] = (input[i] & 0x00FF0000)>>16;
        output[i][2] = (input[i] & 0x0000FF00)>> 8;
        output[i][3] = (input[i] & 0x000000FF)    ;
    }
}

static void make_long_array ( uint8_t input[4][4], uint32_t output[4] )
{
    for ( uint8_t i = 0; i < 4; i++ )
    {   
        output[i] = ((uint32_t)input[i][0])<<24 |
                    ((uint32_t)input[i][1])<<16 |
                    ((uint32_t)input[i][2])<< 8 |
                    ((uint32_t)input[i][3]);
    }
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

    num_keys = get_key_words(keysize);

    num_subkeys = (num_keys+7)<<2;

    for ( unsigned int i = 0; i < num_keys; i++ )
        subkeys[i] = key[i];

    for ( unsigned int i = num_keys; i < num_subkeys; i++ )
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

static void xor_round_key ( uint32_t input[4], uint32_t subkey[4] )
{
    uint32_t tmp_subkey[4] = {0};

    transpose(subkey,tmp_subkey);

    input[0] ^= tmp_subkey[0];
    input[1] ^= tmp_subkey[1];
    input[2] ^= tmp_subkey[2];
    input[3] ^= tmp_subkey[3];
}

static void state_substitute ( uint32_t state[4] )
{
    for ( unsigned int i = 0; i < 4; i++ )
        substitute(&state[i]);
}

static void shiftrows ( uint32_t state[4] )
{
    rol8(&state[1]);
    rol8(&state[2]);
    rol8(&state[2]);
    rol8(&state[3]);
    rol8(&state[3]);
    rol8(&state[3]);
}

static uint8_t dot_product(uint8_t x, uint8_t y)
{
    uint8_t mask = 0x01;
    uint8_t dot_product = 0;
    
    while ( mask )
    {
        if (y&mask)
            dot_product^=x;
        (x&0x80)?(x=((x<<1)^0x1B)):(x<<=1);
        mask<<=1;
    }

    return ( dot_product );
}

static void mixcolumns ( uint32_t state[4] )
{
    uint8_t array[4][4];
    uint8_t tmp[4][4];
    uint32_t tmp_long[4];
    
    make_byte_array(state,array);

    for ( uint8_t i = 0; i < 4; i++ )
    {
        tmp[i][0] = dot_product( 2, array[0][i] ) ^
                    dot_product( 3, array[1][i] ) ^
                                    array[2][i]   ^
                                    array[3][i];

        tmp[i][1] =                 array[0][i]   ^
                    dot_product( 2, array[1][i] ) ^
                    dot_product( 3, array[2][i] ) ^
                                    array[3][i];

        tmp[i][2] =                 array[0][i]   ^
                                    array[1][i]   ^
                    dot_product( 2, array[2][i] ) ^
                    dot_product( 3, array[3][i] );

        tmp[i][3] = dot_product( 3, array[0][i] ) ^
                                    array[1][i]   ^
                                    array[2][i]   ^
                    dot_product( 2, array[3][i] );
    }

    make_long_array ( tmp, tmp_long );

    transpose(tmp_long,state);
}

void aes_encrypt_block ( uint32_t input[4], 
                         uint32_t output[4],
                         uint32_t key[],
                         key_size_t keysize )
{
    uint32_t state[4];
    uint32_t subkeys[60];

    transpose(input,state);
    //print_array("TRANSPOSE", state);

    size_t num_rounds = get_key_words(keysize) + 6;
    //printf("[%lu] ROUNDS\n", num_rounds);

    aes_key_schedule(key, subkeys, keysize);

    xor_round_key(state, &subkeys[0]);

    for ( size_t round = 0; round<(num_rounds-1); round++ )
    {
        state_substitute(state);
        //print_array("STATE-SUB", state);
        shiftrows(state);
        //print_array("SHIFTROWS", state);
        mixcolumns(state); 
        //print_array("MIXCOLUMNS", state);
        xor_round_key(state,&(subkeys[(round+1)<<2]));
    }

    state_substitute(state);
    //print_array("STATE-SUB", state);
    shiftrows(state);
    //print_array("SHIFTROWS", state);
    xor_round_key(state,&(subkeys[(num_rounds)<<2]));

    transpose(state,output);
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

    uint32_t data[4] = {
                          0x01020304,
                          0x05060708,
                          0x090A0B0C,
                          0x0D0E0F10
                       };

    uint32_t output [4] = {0};
    
    for ( i = 0; i < 4; i++ )
        printf ( "DATA[%d]: 0x%08X\n", i, data[i]);

    aes_encrypt_block(data, output, key[0], KEY_128);

    for ( i = 0; i < 4; i++ )
        printf ( "OUT[%d]: 0x%08X\n", i, output[i]);

    aes_encrypt_block(data, output, key[1], KEY_192);

    for ( i = 0; i < 4; i++ )
        printf ( "OUT[%d]: 0x%08X\n", i, output[i]);

    aes_encrypt_block(data, output, key[2], KEY_256);

    for ( i = 0; i < 4; i++ )
        printf ( "OUT[%d]: 0x%08X\n", i, output[i]);


#if 0
    uint32_t subkeys[60];

    for ( i = 0; i < 60; i++ )
        subkeys[i] = 0;

    aes_key_schedule(key[0], subkeys, KEY_128);
    for ( i = 0; i < 60; i++ )
        printf ( "[%d]: 0x%08X\n", i, subkeys[i]);

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

    uint32_t t[4] = { 0x01020304UL, 
                      0x05060708UL, 
                      0x090A0B0CUL,
                      0x0D0E0F10UL };

    uint32_t o[4] = {0};

    for ( i = 0; i < 4; i++ )
        printf ( "[%d]: 0x%08X\n", i, t[i]);

    transpose(t,o);
    for ( i = 0; i < 4; i++ )
        printf ( "[%d]: 0x%08X\n", i, o[i]);
#endif
}
