/*
 *
 * File: convert_tool.c
 * Author: Ron Hume
 * Copyright (c) 2019, All rights Reserved.
 * 
 * A junk file with some old convert routines 
 * that I couldn't throw away after building 
 * the new DES tables.
 */

#include "common.h"

#if 0
/* ORIGINAL DES TABLE (big endian) */
static const uint8_t spec_init_permutation[] = {
    58, 50, 42, 34, 26, 18, 10, 2,
    60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6,
    64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17, 9, 1,
    59, 51, 43, 35, 27, 19, 11, 3, 
    61, 53, 45, 37, 29, 21, 13, 5, 
    63, 55, 47, 39, 31, 23, 15, 7 };

static const uint8_t spec_final_permutation[] = { 
    40, 8, 48, 16, 56, 24, 64, 32,
    39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30,
    37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28,
    35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26,
    33, 1, 41,  9, 49, 17, 57, 25 };

static const uint8_t spec_pc1_table_left[] = {
    57, 49, 41, 33, 25, 17,  9, 1,
    58, 50, 42, 34, 26, 18, 10, 2,
    59, 51, 43, 35, 27, 19, 11, 3,
    60, 52, 44, 36 };

static const uint8_t spec_pc1_table_right[] = {
    63, 55, 47, 39, 31, 23, 15, 7,
    62, 54, 46, 38, 30, 22, 14, 6,
    61, 53, 45, 37, 29, 21, 13, 5,
                    28, 20, 12, 4 };

/* XXXX: recalculate PC2 table */
static const uint8_t spec_pc2_table[] = { 
    14, 17, 11, 24,  1,  5,
     3, 28, 15,  6, 21, 10,
    23, 19, 12,  4, 26,  8,
    16,  7, 27, 20, 13,  2,
    41, 52, 31, 37, 47, 55,
    30, 40, 51, 45, 33, 48,
    44, 49, 39, 56, 34, 53,
    46, 42, 50, 36, 29, 32 };

static void gen_little_endian_permutation_table(const uint8_t tbl[])
{
    uint8_t t[64];

    for ( int i = 0; i < 64; i++ )
    {
        printf ( "%d, ", 64-tbl[i] );
        if ( (i%8) == 7 )
            printf("\n");
        t[63-i] = 64-tbl[i];
    }

    printf("\n=================\n");

    for ( int i = 0; i < 64; i++ )
    {
        printf("%d, ", t[i]);
        if ( (i%8) == 7 )
            printf("\n");
    }
    printf("\n");
}

static void gen_little_endian_pc1_table(const uint8_t tbl[])
{
    uint8_t t[28];

    for ( int i = 0; i < 28; i++ )
    {
        printf ( "%d, ", 64-tbl[i] );
        if ( (i%8) == 7 )
            printf("\n");
        t[27-i] = 64-tbl[i];
    }

    printf("\n=================\n");

    for ( int i = 0; i < 28; i++ )
    {
        printf("%d, ", t[i]);
        if ( (i%8) == 7 )
            printf("\n");
    }
    printf("\n");
}
#endif
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
