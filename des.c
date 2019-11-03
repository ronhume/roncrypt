/*
 *
 * File: des.c
 * Author: Ron Hume
 * Copyright (c) 2019, All rights Reserved.
 * 
 */

#include "common.h"
#include "des.h"
#include "des_tables.h"
#include "fileio.h"

/* 
 * Generic permutation function.
 * Paramters: input, 
 *            output (out param), 
 *            table name, 
 *            number of bits
 *
 * Return: void
 */
static void permutation( uint64_t input, 
                         uint64_t *output, 
                         const uint8_t table[], 
                         size_t num )
{
    *output &= 0x0000000000000000;

    for (uint8_t i = 0; i < num; i++ )
        if (!!((1LL<<table[i]) & input))
            *output |= (1LL<<i);
}

/* 
 * Rotate 28-bit keys left, and combine output
 * Paramters: in_left_key (out param), 
 *            in_right_key (out param), 
 *            out_key (out param)
 *
 * Return: void
 */
static void rol_28_key(uint64_t *in_left, 
                       uint64_t *in_right, 
                       uint64_t *out_key)
{
    *in_left <<= 1LL;
    if (!!(*in_left & 0x0000000010000000))
    {
      *in_left &= 0x000000000FFFFFFF;
      *in_left |= 1LL;
    }

    *in_right <<= 1LL;
    if (!!(*in_right & 0x0000000010000000))
    {
      *in_right &= 0x000000000FFFFFFF;
      *in_right |= 1LL;
    }

    *out_key = (*in_left << 28) | *in_right;
}

/* 
 * Rotate 28-bit keys right, and combine output
 * Paramters: in_left_key (out param), 
 *            in_right_key (out param), 
 *            out_key (out param)
 *
 * Return: void
 */
static void ror_28_key(uint64_t *in_left, 
                       uint64_t *in_right, 
                       uint64_t *out_key)
{
    if (*in_left % 2)
    {
        *in_left >>= 1LL;
        *in_left |= 1LL<<27;
    }
    else    
        *in_left >>= 1LL;

    if (*in_right % 2)
    {
        *in_right >>= 1LL;
        *in_right |= 1LL<<27;
    }
    else    
        *in_right >>= 1LL;

    *out_key = (*in_left << 28) | *in_right;
}

/* 
 * S-BOX operation
 * Paramters: input
 *            output (out param)
 *
 * Return: void
 */
#define NUM_SBOXES 8

static void do_sbox(uint64_t input, uint64_t *output)
{
    *output = 0LL;

    uint8_t bits[NUM_SBOXES];

    /* grab 6-bit valus */
    for ( int i = 0; i < NUM_SBOXES; i++ )
    {
        bits[i] = input & 0x3FLL;
        input >>= 6;
    }

    /* do SBOX substitutions */
    for ( int i = 0; i < NUM_SBOXES; i++ )
    {
        *output |= sbox[i][bits[(NUM_SBOXES-1)-i]];
        *output <<= 4;
    }
    *output >>=4;
}

/* 
 * Generate DES subkeys for encryption
 * Paramters: key
 *            subkeys array (out param)
 *
 * Return: void
 */
static void generate_subkeys_encrypt ( 
              uint64_t key, 
              uint64_t subkey[] )
{
    uint64_t pc1key;
    uint64_t pc1key_left;
    uint64_t pc1key_right;

    permutation(key, &pc1key_left, pc1_table_left, 28);
    permutation(key, &pc1key_right, pc1_table_right, 28);

    pc1key = (pc1key_left << 28) | pc1key_right;

    for ( unsigned int round = 0; round < 16; round++ )
    {
        rol_28_key(&pc1key_left, &pc1key_right, &pc1key);

        if ( !(round == 0 || round == 1 || round ==8 || round == 15))
            rol_28_key(&pc1key_left, &pc1key_right, &pc1key);

        permutation(pc1key, &subkey[round], pc2_table, 48);
    }
}

/* 
 * Generate DES subkeys for decryption
 * Paramters: key
 *            subkeys array (out param)
 *
 * Return: void
 */
static void generate_subkeys_decrypt ( 
              uint64_t key, 
              uint64_t subkey[] )
{
    uint64_t pc1key;
    uint64_t pc1key_left;
    uint64_t pc1key_right;

    permutation(key, &pc1key_left, pc1_table_left, 28);
    permutation(key, &pc1key_right, pc1_table_right, 28);

    pc1key = (pc1key_left << 28) | pc1key_right;

    for ( unsigned int round = 0; round < 16; round++ )
    {
        permutation(pc1key, &subkey[round], pc2_table, 48);
        ror_28_key(&pc1key_left, &pc1key_right, &pc1key);

        if ( !(round == 0 || round == 7 || round ==14 || round == 15))
            ror_28_key(&pc1key_left, &pc1key_right, &pc1key);
    }
}

/* 
 * Perform DES on one 64-bit block
 * Paramters: input - the data
 *            output (out param)
 *            key - the key
 *            mode (encrypt/decrypt)
 *
 * Return: void
 */
static void do_des_block (uint64_t input, 
                          uint64_t *output,
                          uint64_t key,
                          des_mode_t mode )
{
    uint64_t subkey[16];

    uint64_t ip;
    uint64_t expand;
    uint64_t substitute;
    uint64_t permute;

    if ( mode == ENCRYPT )
        generate_subkeys_encrypt(key, subkey);
    else
        generate_subkeys_decrypt(key, subkey);

    permutation(input, &ip, initial_permutation_table, 64);
    
    for ( unsigned int round = 0; round < 16; round++)
    {
        permutation((ip&0x00000000FFFFFFFF), 
                     &expand, 
                     expansion_table, 
                     48);
        
        expand ^= subkey[round];

        /* S-BOXes */
        do_sbox(expand, &substitute);

        permutation(substitute, 
                    &permute, 
                    permutation_table, 32);

        /* swap sides & XOR */
        uint64_t tmp = (ip>>32);
        tmp ^= permute;
        ip = (ip<<32)|tmp;
    }

    ip = (ip<<32)|(ip>>32);

    permutation ( ip, output, final_permutation_table, 64 );
}

/* 
 * Perform DES on one 64-bit block
 * Paramters: input - the data
 *            output (out param)
 *            key - the key
 *            salt - the salt (for CBC)
 *            mode (encrypt/decrypt)
 *
 * Return: void
 */
static void do_des (uint64_t input, 
                    uint64_t *output,
                    uint64_t key,
                    uint64_t salt,
                    des_mode_t mode )
{

    if (mode == ENCRYPT)
        input ^= salt;

    do_des_block( input, output, key, mode ); 

    if (mode == DECRYPT)
        *output ^= salt;
}

/* 
 * Perform 3DES on one 64-bit block
 * Paramters: input - the data
 *            output (out param)
 *            key - array of 3 64bit keys
 *            salt - the salt (for CBC)
 *            mode (encrypt/decrypt)
 *
 * Return: void
 */
static void do_3des (uint64_t input, 
                     uint64_t *output,
                     uint64_t key[],
                     uint64_t salt,
                     des_mode_t mode )
{
    uint64_t output2 = 0LL;
    uint64_t output3 = 0LL;

    if (mode == ENCRYPT)
    {
        input ^= salt;

        do_des_block( input, &output3, key[0], mode ); 
        do_des_block( output3, &output2, key[1], DECRYPT ); 
        do_des_block( output2, output, key[2], mode ); 
    }
    

    if (mode == DECRYPT)
    {
        do_des_block( input, &output3, key[2], mode ); 
        do_des_block( output3, &output2, key[1], ENCRYPT ); 
        do_des_block( output2, output, key[0], mode ); 

        *output ^= salt;
    }
}

/* 
 * Perform DES on 64-bit array of blocks
 * Paramters: input - the data
 *            output - encrypted blocks
 *            length - how much input
 *            key - DES key
 *            salt - the salt (for CBC)
 *            mode (encrypt/decrypt)
 *
 * Return: void
 */
void des( uint64_t *input, 
          uint64_t *output,
          size_t  length,
          uint64_t key,
          uint64_t salt,
          des_mode_t mode )
{
    for ( int i = 0; i < length; i++ )
    {
        do_des( input[i], &output[i], key, salt, mode ); 
        if ( mode == ENCRYPT )
            salt = output[i];
        else
            salt = input[i];
    }
}

/* 
 * Perform 3DES on 64-bit array of blocks
 * Paramters: input - the data
 *            output - encrypted blocks
 *            length - how much input
 *            key - 3DES key array of 3 keys
 *            salt - the salt (for CBC)
 *            mode (encrypt/decrypt)
 *
 * Return: void
 */
void tripledes( uint64_t *input, 
                uint64_t *output,
                size_t  length,
                uint64_t key[],
                uint64_t salt,
                des_mode_t mode )
{
    for ( int i = 0; i < length; i++ )
    {
        do_3des( input[i], &output[i], key, salt, mode ); 
        if ( mode == ENCRYPT )
            salt = output[i];
        else
            salt = input[i];
    }
}

void des_file( int in_fd, 
               int out_fd,
               uint64_t key,
               uint64_t salt,
               des_mode_t mode )
{
    bool pad_full_block = false;
    uint64_t input, output;

    size_t filesize = lseek(in_fd, 0L, SEEK_END);
    lseek (in_fd, 0L, SEEK_SET);

    /* PKCS #5/#7 padding */
    if (mode == ENCRYPT)
    {
        if (!(filesize % DES_BLKSZ))
        {
            pad_full_block = true;
            filesize+=DES_BLKSZ;
        }
        else
        {
            filesize+=(DES_BLKSZ-(filesize%DES_BLKSZ));
        }
    }

    while (des_readblock(in_fd, &input)>0)
    {
        do_des( input, &output, key, salt, mode ); 
        if ( mode == ENCRYPT )
            salt = output;
        else
            salt = input;

        des_writeblock(out_fd, output);
    }

    if (mode == ENCRYPT && pad_full_block)
    {
        input = 0x0808080808080808LL;
        do_des( input, &output, key, salt, mode ); 
        des_writeblock(out_fd, output);
    }

    if (mode == DECRYPT)
    {
        /* remove padding */
        size_t how_much = (size_t)(output & 0x00000000000000FFLL);
        size_t newsize = filesize-how_much;
        ftruncate(out_fd, newsize);
    }
}

void tripledes_file( int in_fd, 
               int out_fd,
               uint64_t key[],
               uint64_t salt,
               des_mode_t mode )
{
    bool pad_full_block = false;
    uint64_t input, output;

    size_t filesize = lseek(in_fd, 0L, SEEK_END);
    lseek (in_fd, 0L, SEEK_SET);

    /* PKCS #5/#7 padding */
    if ( mode == ENCRYPT)
    {
        if (!(filesize % DES_BLKSZ))
        {
            pad_full_block = true;
            filesize+=DES_BLKSZ;
        }
        else
        {
            filesize+=(DES_BLKSZ-(filesize%DES_BLKSZ));
        }
    }

    while (des_readblock(in_fd, &input)>0)
    {
        do_3des( input, &output, key, salt, mode ); 
        if ( mode == ENCRYPT )
            salt = output;
        else
            salt = input;

        des_writeblock(out_fd, output);
    }

    if ( (mode == ENCRYPT) && pad_full_block )
    {
        input = 0x0808080808080808LL;
        do_3des( input, &output, key, salt, mode ); 
        des_writeblock(out_fd, output);
    }

    if (mode == DECRYPT )
    {
        /* remove padding */
        size_t how_much = (size_t)(output & 0x00000000000000FFLL);
        size_t newsize = filesize-how_much;
        ftruncate(out_fd, newsize);
    }
}

