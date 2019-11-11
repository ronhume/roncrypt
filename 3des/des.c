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

#define ONE_BIT (1ULL)

/* 
 * Generic permutation function.
 * Parameters: input, 
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
    register uint64_t out = 0;

    for (uint8_t i = 0; i < num; i++ )
        out |= ((input>>table[i])&ONE_BIT)<<i;

    *output = out;
}

/* 
 * Rotate 28-bit keys left, and combine output
 * Parameters: in_left_key (out param), 
 *            in_right_key (out param), 
 *            out_key (out param)
 *
 * Return: void
 */
static void rol_28_key(uint64_t *in_left, 
                       uint64_t *in_right, 
                       uint64_t *out_key)
{
    *in_left <<= 1;
    if (!!(*in_left & 0x0000000010000000ULL))
      *in_left ^= 0x0000000010000001ULL;

    *in_right <<= 1;
    if (!!(*in_right & 0x0000000010000000ULL))
      *in_right ^= 0x0000000010000001ULL;

    *out_key = (*in_left << 28) | *in_right;
}

/* 
 * Rotate 28-bit keys right, and combine output
 * Parameters: in_left_key (out param), 
 *            in_right_key (out param), 
 *            out_key (out param)
 *
 * Return: void
 */
static void ror_28_key(uint64_t *in_left, 
                       uint64_t *in_right, 
                       uint64_t *out_key)
{
    if (*in_left & ONE_BIT)
        *in_left |= ONE_BIT<<28;

    *in_left >>= 1;

    if (*in_right & ONE_BIT)
        *in_right |= ONE_BIT<<28;

    *in_right >>= 1;

    *out_key = (*in_left << 28) | *in_right;
}

/* 
 * S-BOX operation
 * Parameters: input
 *            output (out param)
 *
 * Return: void
 */
#define NUM_SBOXES 8

static void do_sbox(uint64_t input, uint64_t *output)
{
    register uint64_t out = 0;
    uint8_t bits[NUM_SBOXES];

    /* grab 6-bit valus */
    for ( int i = 0; i < NUM_SBOXES; i++ )
    {
        bits[i] = input & 0x3FULL;
        input >>= 6;
    }

    /* do SBOX substitutions */
    for ( int i = 0; i < NUM_SBOXES; i++ )
    {
        out <<= 4;
        out |= sbox[i][bits[(NUM_SBOXES-1)-i]];
    }

    *output = out;
}

/* 
 * Generate DES subkeys for encryption
 * Parameters: key
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
 * Parameters: key
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
 * Generate subkeys from key for 3DES
 * Parameters: key - key array of 3 DES keys
 *            subkey (out param) - 3 arrays of 16 subkeys
 *            mode (encrypt/decrypt)
 *
 * Return: void
 */
static void generate_subkeys_3des(
                uint64_t key[],
                uint64_t subkey[][16],
                des_mode_t mode )
{
    if ( mode == ENCRYPT )
    {
        generate_subkeys_encrypt(key[0], subkey[0]);
        generate_subkeys_decrypt(key[1], subkey[1]);
        generate_subkeys_encrypt(key[2], subkey[2]);
    }
    else
    {
        generate_subkeys_decrypt(key[0], subkey[0]);
        generate_subkeys_encrypt(key[1], subkey[1]);
        generate_subkeys_decrypt(key[2], subkey[2]);
    }
}

/* 
 * Perform DES on one 64-bit block
 * Parameters: input - the data
 *            output (out param)
 *            subkey - subkey array of 16 subkeys
 *            mode (encrypt/decrypt)
 *
 * Return: void
 */
static void do_des_block (uint64_t input, 
                          uint64_t *output,
                          uint64_t subkey[],
                          des_mode_t mode )
{
    uint64_t ip;
    uint64_t expand;
    uint64_t substitute;
    uint64_t permute;

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
 * Parameters: input - the data
 *            output (out param)
 *            subkey - array of 16 subkeys
 *            salt - the salt (for CBC)
 *            mode (encrypt/decrypt)
 *
 * Return: void
 */
static void do_des (uint64_t input, 
                    uint64_t *output,
                    uint64_t subkey[],
                    uint64_t salt,
                    des_mode_t mode )
{
    if (mode == ENCRYPT)
        input ^= salt;

    do_des_block( input, output, subkey, mode ); 

    if (mode == DECRYPT)
        *output ^= salt;
}

/* 
 * Perform 3DES on one 64-bit block
 * Parameters: input - the data
 *            output (out param)
 *            subkey - array of 3 16 subkey arrays
 *            salt - the salt (for CBC)
 *            mode (encrypt/decrypt)
 *
 * Return: void
 */
static void do_3des (uint64_t input, 
                     uint64_t *output,
                     uint64_t subkey[][16],
                     uint64_t salt,
                     des_mode_t mode )
{
    uint64_t output2 = 0ULL;
    uint64_t output3 = 0ULL;

    if (mode == ENCRYPT)
    {
        input ^= salt;

        do_des_block( input, &output3, subkey[0], mode ); 
        do_des_block( output3, &output2, subkey[1], DECRYPT ); 
        do_des_block( output2, output, subkey[2], mode ); 
    }
    

    if (mode == DECRYPT)
    {
        do_des_block( input, &output3, subkey[2], mode ); 
        do_des_block( output3, &output2, subkey[1], ENCRYPT ); 
        do_des_block( output2, output, subkey[0], mode ); 

        *output ^= salt;
    }
}

/* 
 * Perform DES on 64-bit array of blocks
 * Parameters: input - the data
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
    uint64_t subkey[16];
    if ( mode == ENCRYPT )
        generate_subkeys_encrypt(key, subkey);
    else
        generate_subkeys_decrypt(key, subkey);

    for ( int i = 0; i < length; i++ )
    {
        do_des( input[i], &output[i], subkey, salt, mode ); 
        if ( mode == ENCRYPT )
            salt = output[i];
        else
            salt = input[i];
    }
}

/* 
 * Perform 3DES on 64-bit array of blocks
 * Parameters: input - the data
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
    uint64_t subkey[3][16];

    generate_subkeys_3des(key, subkey, mode);

    for ( int i = 0; i < length; i++ )
    {
        do_3des( input[i], &output[i], subkey, salt, mode ); 
        if ( mode == ENCRYPT )
            salt = output[i];
        else
            salt = input[i];
    }
}

/* 
 * Perform DES on files
 * Parameters: in - input file name
 *            out - output file name
 *            key - DES key
 *            salt - the salt (for CBC)
 *            mode (encrypt/decrypt)
 *
 * Return: void
 */
void des_file( const char* const in, 
               const char* const out,
               uint64_t key,
               uint64_t salt,
               des_mode_t mode )
{
    bool pad_full_block = false;
    uint64_t input, output;
    uint64_t subkey[16];
 
    int infile = openinfile(in);
    int outfile = openoutfile(out);

    size_t filesize = lseek(infile, 0L, SEEK_END);
    lseek (infile, 0L, SEEK_SET);

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

    char* bufptr;
    size_t bufsz;

    /* make reasonablly sized, page-aligned output buffer */
    if ( filesize < 4097 )
        bufsz = 4096;
    else
        bufsz = (filesize - (filesize % 2048))>>2;

    /* open output file for buffered I/O */
    FILE* out_fp = outfile_buffered(outfile, bufsz, &bufptr);

    if ( mode == ENCRYPT )
        generate_subkeys_encrypt(key, subkey);
    else
        generate_subkeys_decrypt(key, subkey);

    while (des_readblock(infile, &input)>0)
    {
        do_des( input, &output, subkey, salt, mode ); 
        if ( mode == ENCRYPT )
            salt = output;
        else
            salt = input;

        des_writeblock_buffered(out_fp, output);
    }

    if (mode == ENCRYPT && pad_full_block)
    {
        input = 0x0808080808080808ULL;
        do_des( input, &output, subkey, salt, mode ); 
        des_writeblock_buffered(out_fp, output);
    }

    if (mode == DECRYPT)
    {
        fflush(out_fp);
        closefile_buffered(out_fp, (void**)&bufptr);

        /* remove padding */
        size_t how_much = (size_t)(output & 0x00000000000000FFULL);
        size_t newsize = filesize-how_much;
        truncate(out, newsize);
    }
    else
    {
        closefile_buffered(out_fp, (void**)&bufptr);
    }

    closefile(infile);
}

/* 
 * Perform 3DES on files
 * Parameters: in - input file name
 *            out - output file name
 *            key - 3DES key array of 3 keys
 *            salt - the salt (for CBC)
 *            mode (encrypt/decrypt)
 *
 * Return: void
 */
void tripledes_file( const char* const in, 
                     const char* const out,
                     uint64_t key[],
                     uint64_t salt,
                     des_mode_t mode )
{
    bool pad_full_block = false;
    uint64_t input, output;
    uint64_t subkey[3][16];

    int infile = openinfile(in);
    int outfile = openoutfile(out);

    size_t filesize = lseek(infile, 0L, SEEK_END);
    lseek (infile, 0L, SEEK_SET);

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

    char* bufptr;
    size_t bufsz;

    /* make reasonably size, page-aligned output buffer */
    if ( filesize < 4097 )
        bufsz = 4096;
    else
        bufsz = (filesize - (filesize % 2048))>>2;

    /* open output file for buffered I/O */
    FILE* out_fp = outfile_buffered(outfile, bufsz, &bufptr);

    generate_subkeys_3des(key, subkey, mode);

    while (des_readblock(infile, &input)>0)
    {
        do_3des( input, &output, subkey, salt, mode ); 
        if ( mode == ENCRYPT )
            salt = output;
        else
            salt = input;

        des_writeblock_buffered(out_fp, output);
    }

    if ( (mode == ENCRYPT) && pad_full_block )
    {
        input = 0x0808080808080808ULL;
        do_3des( input, &output, subkey, salt, mode ); 
        des_writeblock_buffered(out_fp, output);
    }

    if (mode == DECRYPT)
    {
        fflush(out_fp);
        closefile_buffered(out_fp, (void**)&bufptr);

        /* remove padding */
        size_t how_much = (size_t)(output & 0x00000000000000FFULL);
        size_t newsize = filesize-how_much;
        truncate(out, newsize);
    }
    else
    {
        closefile_buffered(out_fp, (void**)&bufptr);
    }

    closefile(infile);
}

