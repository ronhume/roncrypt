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

void des_encrypt_block (uint64_t input, 
                  uint64_t *output,
                  uint64_t cbc_iv,
                  uint64_t key )
{
    uint64_t ip_block;
    uint64_t expansion_block;
    uint64_t substitution_block;
    uint64_t pbox_target;
    uint64_t recomb_box;
    uint64_t pc1key;
    uint64_t pc1key_left;
    uint64_t pc1key_right;
    uint64_t subkey;

    unsigned int round;

    printf ( "IV: 0x%" PRIx64 "\n", cbc_iv);
    printf ( "INPUT: 0x%" PRIx64 "\n", input);
    input ^= cbc_iv;
    printf ( "INPUT_IV: 0x%" PRIx64 "\n", input);
    permutation(input, &ip_block, initial_permutation_table, 64);
    printf ( "PERMUTED(IP): 0x%" PRIx64 "\n", ip_block);

    printf ( "KEY: 0x%" PRIx64 "\n", key);
    permutation(key, &pc1key_left, pc1_table_left, 28);
    //printf ( "PC1 LEFT : 0x%" PRIx64 "\n", pc1key_left);
    permutation(key, &pc1key_right, pc1_table_right, 28);
    //printf ( "PC1 RIGHT: 0x%" PRIx64 "\n", pc1key_right);
    pc1key = (pc1key_left << 28) | pc1key_right;
    printf ( "PC1 KEY: 0x%" PRIx64 "\n", pc1key);
    
    for ( int round = 0; round < 16; round++)
    {
        printf ("ROUND [%d]\n", round);

        uint64_t in_block = ip_block&0x00000000FFFFFFFF;
        //printf ( "LOWER INPUT: 0x%" PRIx64 "\n", in_block);
        permutation(in_block, &expansion_block, expansion_table, 48);
        printf ( "EXPANSION : 0x%" PRIx64 "\n", expansion_block);
        
        rol_28_key(&pc1key_left, &pc1key_right, &pc1key);
        //printf ( "PC1 LEFT : 0x%" PRIx64 "\n", pc1key_left);
        //printf ( "PC1 RIGHT: 0x%" PRIx64 "\n", pc1key_right);
        printf ( "PC1 KEY: 0x%" PRIx64 "\n", pc1key);
        if ( !(round == 0 || round == 1 || round ==8 || round == 15))
        {
            rol_28_key(&pc1key_left, &pc1key_right, &pc1key);
            printf ( "PC1 KEY: 0x%" PRIx64 "\n", pc1key);
        }

        permutation(pc1key, &subkey, pc2_table, 48);
        printf ( "SUBKEY : 0x%" PRIx64 "\n", subkey);
        expansion_block ^= subkey;
        printf ( "EXPANSION(XOR) : 0x%" PRIx64 "\n", expansion_block);

        /* S-BOXes */
        do_sbox(expansion_block,&substitution_block);
        printf ( "SBOX: 0x%" PRIx64 "\n", substitution_block);

        permutation(substitution_block, &pbox_target, permutation_table, 32);
        printf ( "PBOX: 0x%" PRIx64 "\n", pbox_target);

        /* swap sides */
        uint64_t tmp_var = (ip_block>>32LL);
        tmp_var ^= pbox_target;
        ip_block = (ip_block<<32LL)|tmp_var;
        
        printf ( "OUTPUT: 0x%" PRIx64 "\n", ip_block);
    }

    uint64_t tmp_var = (ip_block>>32LL);
    ip_block = (ip_block<<32LL)|tmp_var;

    permutation ( ip_block, output, final_permutation_table, 64 );
}

int main()
{
    //unsigned char key[] = "password";
    uint64_t key = 0x70617373776F7264;
    //unsigned char cbc_iv[] = "initialz";
    uint64_t cbc_iv = 0x696E697469616C7A;
    //unsigned char input[] = "abcdefgh";
    uint64_t input = 0x6162636465666768 ;

#if 0
    for (int i = 0; i<8; i++)
        printf("0x%02X ", input[i]);
    printf ("\n");
    for (int i = 0; i<8; i++)
        printf("0x%02X ", key[i]);
    printf ("\n");
    //for (int i = 0; i<8; i++)
    //    printf("0x%02X ", key[i]);
#endif

    uint64_t output = 0LL;

    des_encrypt_block( input, &output, cbc_iv, key ); 
    printf ( "DES: 0x%" PRIx64 "\n", output);

}

#if 0
int main()
{
    //gen_little_endian_permutation_table(spec_init_permutation);
    //gen_little_endian_permutation_table(spec_final_permutation);
    //gen_little_endian_pc1_table(spec_pc1_table_left);
    //gen_little_endian_pc1_table(spec_pc1_table_right);

    //uint64_t input = 0x0123456789ABCDEF;
    uint64_t input =   0x4142434445464748;
    uint64_t pc1_input = 0x4142434445464700;
    uint64_t output = 0;
    uint64_t output2 = 0;
    uint64_t output3 = 0;

#if 0
    /* initial and final permutations */
    permutation(input, &output, initial_permutation_table, 64);
    printf ( "input: 0x%" PRIx64 "\npermuted: 0x%" PRIx64 "\n", input, output);
    permutation(output, &output2, final_permutation_table, 64);
    printf ( "unpermuted: 0x%" PRIx64 "\n", output2 );
    if ( input == output2 )
        printf ( "SUCCESS!\n" );
    else
        printf ( "FAILURE!\n" );
    /* PC1 left and right */
#endif
    permutation(pc1_input, &output, pc1_table_left, 28);
    permutation(pc1_input, &output2, pc1_table_right, 28);
    printf ( "input: 0x%" PRIx64 "\npermuted: 0x%" PRIx64 "\n", pc1_input, output);
    printf ( "input: 0x%" PRIx64 "\npermuted: 0x%" PRIx64 "\n", pc1_input, output2);
    output3 = (output<<28)|output2;
    printf ( "input: 0x%" PRIx64 "\npermuted: 0x%" PRIx64 "\n", pc1_input, output3);
}
#endif
