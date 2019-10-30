/*
 *
 * File: des.c
 * Author: Ron Hume
 * Copyright (c) 2019, All rights Reserved.
 * 
 */

#include "common.h"
#include "des_tables.h"

typedef enum { DES_ENCRYPT, DES_DECRYPT } des_mode_t;

/* 
 * Generic permutation function.
 * Paramters: input, 
 *            output (out param), 
 *            table name, 
 *            number of bits
 *
 * Return: void
 */
void permutation( uint64_t input, 
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
void rol_28_key(uint64_t *in_left, 
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
void ror_28_key(uint64_t *in_left, 
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

void do_sbox(uint64_t input, uint64_t *output)
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
void generate_subkeys_encrypt ( uint64_t key, uint64_t subkey[] )
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
void generate_subkeys_decrypt ( uint64_t key, uint64_t subkey[] )
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
void do_des_block (uint64_t input, 
                  uint64_t *output,
                  uint64_t key,
                  des_mode_t mode )
{
    uint64_t subkey[16];

    uint64_t ip;
    uint64_t expand;
    uint64_t substitute;
    uint64_t permute;

    if ( mode == DES_ENCRYPT )
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
void do_des (uint64_t input, 
             uint64_t *output,
             uint64_t key,
             uint64_t salt,
             des_mode_t mode )
{

    if (mode == DES_ENCRYPT)
        input ^= salt;

    do_des_block( input, output, key, mode ); 

    if (mode == DES_DECRYPT)
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
void do_3des (uint64_t input, 
             uint64_t *output,
             uint64_t key[],
             uint64_t salt,
             des_mode_t mode )
{
    uint64_t output2 = 0LL;
    uint64_t output3 = 0LL;

    if (mode == DES_ENCRYPT)
    {
        input ^= salt;

        do_des_block( input, &output3, key[0], mode ); 
        do_des_block( output3, &output2, key[1], DES_DECRYPT ); 
        do_des_block( output2, output, key[2], mode ); 
    }
    

    if (mode == DES_DECRYPT)
    {
        do_des_block( input, &output3, key[2], mode ); 
        do_des_block( output3, &output2, key[1], DES_ENCRYPT ); 
        do_des_block( output2, output, key[0], mode ); 

        *output ^= salt;
    }

    /* XXXX: TODO: pass back CBC residue? */
}

int main()
{
    //unsigned char key[] = "password";
    //unsigned char key_3des[] = "twentyfourcharacterinput";
    //unsigned char salt[] = "initialz";
    //unsigned char input[] = "abcdefgh";

    uint64_t key = 0x70617373776F7264;
    uint64_t key_3des[3] = {0x7477656E7479666F,
                            0x7572636861726163,
                            0x746572696E707574};
    uint64_t salt = 0x696E697469616C7A;
    uint64_t input = 0x6162636465666768 ;

    uint64_t output = 0LL;
    uint64_t output2 = 0LL;

    do_des( input, &output, key, salt, DES_ENCRYPT ); 
    printf ( "DES: 0x%" PRIx64 "\n", output);

    do_des( output, &output2, key, salt, DES_DECRYPT );
    printf ( "PT: 0x%" PRIx64 "\n", output2);

    do_3des( input, &output, key_3des, salt, DES_ENCRYPT ); 
    printf ( "3DES: 0x%" PRIx64 "\n", output);

    do_3des( output, &output2, key_3des, salt, DES_DECRYPT );
    printf ( "PT: 0x%" PRIx64 "\n", output2);

}

