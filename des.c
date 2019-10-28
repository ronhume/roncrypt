/*
 *
 * File: des.c
 * Author: Ron Hume
 * Copyright (c) 2019, All rights Reserved.
 * 
 */

#include "common.h"
#include "des_tables.h"

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
 * Generate DES subkeys
 * Paramters: key
 *            subkeys array (out param)
 *
 * Return: void
 */
void generate_subkeys ( uint64_t key, uint64_t subkey[] )
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

void des_encrypt_block (uint64_t input, 
                  uint64_t *output,
                  uint64_t key )
{
    uint64_t subkey[16];

    uint64_t ip;
    uint64_t expand;
    uint64_t substitute;
    uint64_t permute;

    generate_subkeys(key, subkey);

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

int main()
{
    //unsigned char key[] = "password";
    uint64_t key = 0x70617373776F7264;
    //unsigned char cbc_iv[] = "initialz";
    uint64_t cbc_iv = 0x696E697469616C7A;
    //unsigned char input[] = "abcdefgh";
    uint64_t input = 0x6162636465666768 ;

    uint64_t output = 0LL;

    input ^= cbc_iv;
    des_encrypt_block( input, &output, key ); 
    printf ( "DES: 0x%" PRIx64 "\n", output);
}

