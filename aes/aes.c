/*
 *
 * File: aes.c
 * Author: Ron Hume
 * Copyright (c) 2019, All rights Reserved.
 * 
 */

#include "common.h"
#include "aes.h"
#include "aes_tables.h"
#include "fileio.h"

#define WORD_MASK 0x00000000FFFFFFFFULL;

/* 
 * Get number of words in key
 * Parameters: size - key size enum
 *
 * Return: number of words in key
 */
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

#if 0
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
#endif

/* 
 * Rotate word left 8
 * Parameters: data - pointer to data
 *
 * Return: void
 */
static void rol8 ( uint32_t *data )
{
    register uint64_t tmp_data = (*data);
    uint8_t tmp = tmp_data>>24;

    tmp_data = (tmp_data<<40)>>32;
    tmp_data += tmp;
    *data = (uint32_t)tmp_data;
}

/* 
 * Transpose 4x4 matix of bytes passed 
 * as 4 32bit words
 * Parameters: input - word array
 *             output - word array
 *
 * Return: void
 */
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

/* 
 * Convert 4 word array into 4x4 byte array
 * Parameters: input - word array
 *             output - byte array
 *
 * Return: void
 */
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

/* 
 * Convert 4x4 byte array into array of words
 * Parameters: input - byte array
 *             output - word array
 *
 * Return: void
 */
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

/* 
 * aes SBOX substitution
 * Parameters: data - pointer to word
 *             invert - which sbox table to use.
 *
 * Return: void
 */
static void substitute ( uint32_t *data, bool invert )
{
    if ( invert )
    {
        *data = aes_inv_sbox[ ((*data)     >>24)]<<24 |
                aes_inv_sbox[(((*data)<< 8)>>24)]<<16 |
                aes_inv_sbox[(((*data)<<16)>>24)]<< 8 |
                aes_inv_sbox[(((*data)<<24)>>24)]     ;
    }
    else
    {
        *data = aes_sbox[ ((*data)     >>24)]<<24 |
                aes_sbox[(((*data)<< 8)>>24)]<<16 |
                aes_sbox[(((*data)<<16)>>24)]<< 8 |
                aes_sbox[(((*data)<<24)>>24)]     ;
    }
}

/* 
 * Generate AES key schedule
 * Parameters: key - key data array
 *             subkeys - subkey array data (out param)
 *             keysize - how much key data to use
 *
 * Return: void
 */
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
            substitute (&(subkeys[i]),false);
        
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
            substitute (&(subkeys[i]),false);
        }

        subkeys[i] ^= subkeys[i-num_keys];
    }
}

/* 
 * XOR round key into data
 * Parameters: input - word array
 *             subkey - subkey array material
 *
 * Return: void
 */
static void xor_round_key ( uint32_t input[4], uint32_t subkey[4] )
{
    uint32_t tmp_subkey[4] = {0};

    transpose(subkey,tmp_subkey);

    input[0] ^= tmp_subkey[0];
    input[1] ^= tmp_subkey[1];
    input[2] ^= tmp_subkey[2];
    input[3] ^= tmp_subkey[3];
}

/* 
 * State array substituion
 * Parameters: state - state array
 *             invert - which direction
 *
 * Return: void
 */
static void state_substitute ( uint32_t state[4], bool invert )
{
    for ( unsigned int i = 0; i < 4; i++ )
        substitute(&state[i],invert);
}

/* 
 * ShiftRows AES operation
 * Parameters: state - state array
 *             invert - which direction
 *
 * Return: void
 */
static void shiftrows ( uint32_t state[4], bool invert )
{
    if ( invert)
    {
        rol8(&state[1]);
        rol8(&state[1]);
        rol8(&state[1]);
        rol8(&state[2]);
        rol8(&state[2]);
        rol8(&state[3]);
    }
    else
    {
        rol8(&state[1]);
        rol8(&state[2]);
        rol8(&state[2]);
        rol8(&state[3]);
        rol8(&state[3]);
        rol8(&state[3]);
    }
}

/* 
 * Calculate AES' weird dot_product
 * Parameters: x,y
 *
 * Return: dot_product
 */
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

/* 
 * AES MixColumns operation
 * Parameters: state - state array
 *
 * Return: void
 */
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

/* 
 * AES encrypt one block
 * Parameters: input - data input array
 *             output - data output array
 *             key - key material
 *             keysize - key size enum
 *
 * Return: void
 */
static void aes_encrypt_block ( uint32_t input[4], 
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
        state_substitute(state, false);
        //print_array("STATE-SUB", state);
        shiftrows(state, false);
        //print_array("SHIFTROWS", state);
        mixcolumns(state); 
        //print_array("MIXCOLUMNS", state);
        xor_round_key(state,&(subkeys[(round+1)<<2]));
    }

    state_substitute(state, false);
    //print_array("STATE-SUB", state);
    shiftrows(state, false);
    //print_array("SHIFTROWS", state);
    xor_round_key(state,&(subkeys[(num_rounds)<<2]));

    transpose(state,output);
}

/* 
 * AES encrypt one block with salt.  This
 * method is to support CBC mode.
 * Parameters: input - input data array
 *             output - output data array
 *             salt - iv for CBC
 *             key - key material array
 *             keysize - key size enum
 *
 * Return: void
 */
static void aes_encrypt_block_cbc ( uint32_t input[4], 
                             uint32_t output[4],
                             uint32_t salt[4],
                             uint32_t key[],
                             key_size_t keysize )
{
    uint32_t data[4];
    data[0] = input[0] ^ salt[0];
    data[1] = input[1] ^ salt[1];
    data[2] = input[2] ^ salt[2];
    data[3] = input[3] ^ salt[3];

    aes_encrypt_block(data, output, key, keysize);
}

/* 
 * Do AES on a buffer
 * Parameters: input - data pointer
 *             output - output data pointer
 *             length - length of input
 *             key - key array
 *             salt - iv for CBC
 *             keysize - key size enum
 *
 * Return: void
 */
void aes (uint32_t *input, 
          uint32_t *output,
          size_t    length,
          uint32_t  key[],
          uint32_t  salt[4],
          key_size_t keysize)
{    
    /* generate subkeys here? XXXX */

    for (size_t i = 0; i<length; i+=4)
    {
        aes_encrypt_block_cbc(&(input[i]), 
                              &(output[i]), 
                                salt, 
                                key, 
                                keysize);

        salt[0] = output[i];
        salt[1] = output[i+1];
        salt[2] = output[i+2];
        salt[3] = output[i+3];
    }
}

/* 
 * AES for files
 * Parameters: in - filename of input
 *             out - filename of output file
 *             key - key array
 *             salt - IV for CBC
 *             keysize - key size enum
 *             mode - encrypt / decrypt
 *
 * Return: void
 */
void aes_file( const char* const in,
               const char* const out,
               uint32_t key[],
               uint32_t salt[4],
               key_size_t keysize,
               aes_mode_t mode)
{
    bool pad_full_block = false;
    uint32_t input[4];
    uint32_t output[4];
    
    int infile = openinfile(in);
    int outfile = openoutfile(out);

    size_t filesize = lseek(infile, 0L, SEEK_END);
    lseek (infile, 0L, SEEK_SET);

    /* PKCS #5/#7 padding */
    if (mode == ENCRYPT)
    {
        if (!(filesize % AES_BLKSZ ))
        {
            pad_full_block = true;
            filesize+=AES_BLKSZ;
        }
        else
        {
            filesize+=(AES_BLKSZ-(filesize%AES_BLKSZ));
        }
    }

    char* bufptr;
    size_t bufsz;

    /* make reasonably sized, page-aligned output buffer */
    if ( filesize < 4097 )
        bufsz = 4096;
    else
        bufsz = (filesize - (filesize % 2048))>>2;

    /* open output file for buffered I/O */
    FILE* out_fp = outfile_buffered(outfile, bufsz, &bufptr);

    /* XXXX: generate subkeys here? */

    while(aes_readblock(infile, input)>0)
    {
        aes_encrypt_block_cbc(input,output, salt, key, keysize);
        if ( mode == ENCRYPT )
        {
            salt[0] = output[0];
            salt[1] = output[1];
            salt[2] = output[2];
            salt[3] = output[3];
        }
        else
        {
            salt[0] = input[0];
            salt[1] = input[1];
            salt[2] = input[2];
            salt[3] = input[3];
        }

        aes_writeblock_buffered(out_fp, output);
    }

    if (mode == ENCRYPT && pad_full_block)
    {
        input[0] = 0x10101010UL;
        input[1] = 0x10101010UL;
        input[2] = 0x10101010UL;
        input[3] = 0x10101010UL;
        aes_encrypt_block_cbc(input, output, salt, key, keysize);
        aes_writeblock_buffered(out_fp, output);
    }

    if ( mode == DECRYPT )
    {
        /* TODO:XXXX */
    }
    else
    {
        closefile_buffered(out_fp, (void**)&bufptr);
    }

    closefile(infile);
}

