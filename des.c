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

/* RECALCUALTED TABLES (little-endian/zero indexed) */

static const uint8_t initial_permutation_table[] = {
    57, 49, 41, 33, 25, 17, 9, 1, 
    59, 51, 43, 35, 27, 19, 11, 3, 
    61, 53, 45, 37, 29, 21, 13, 5, 
    63, 55, 47, 39, 31, 23, 15, 7, 
    56, 48, 40, 32, 24, 16, 8, 0, 
    58, 50, 42, 34, 26, 18, 10, 2, 
    60, 52, 44, 36, 28, 20, 12, 4, 
    62, 54, 46, 38, 30, 22, 14, 6 };

static const uint8_t  final_permutation_table[] = {
    39, 7, 47, 15, 55, 23, 63, 31, 
    38, 6, 46, 14, 54, 22, 62, 30, 
    37, 5, 45, 13, 53, 21, 61, 29, 
    36, 4, 44, 12, 52, 20, 60, 28, 
    35, 3, 43, 11, 51, 19, 59, 27, 
    34, 2, 42, 10, 50, 18, 58, 26, 
    33, 1, 41, 9, 49, 17, 57, 25, 
    32, 0, 40, 8, 48, 16, 56, 24 };

static const uint8_t pc1_table_left[] = {
    28, 20, 12, 4, 61, 53, 45, 37, 
    29, 21, 13, 5, 62, 54, 46, 38, 
    30, 22, 14, 6, 63, 55, 47, 39, 
    31, 23, 15, 7 };

static const uint8_t pc1_table_right[] = {
    60, 52, 44, 36, 59, 51, 43, 35, 
    27, 19, 11,  3, 58, 50, 42, 34, 
    26, 18, 10,  2, 57, 49, 41, 33, 
    25, 17,  9,  1 };

static const uint8_t pc2_table[] = { 
#if 0
    14, 17, 11, 24,  1,  5,
     3, 28, 15,  6, 21, 10,
    23, 19, 12,  4, 26,  8,
    16,  7, 27, 20, 13,  2,
    41, 52, 31, 37, 47, 55,
    30, 40, 51, 45, 33, 48,
    44, 49, 39, 56, 34, 53,
    46, 42, 50, 36, 29, 32 };
#else
    24, 27, 20,  6, 14, 10,
     3, 22,  0, 17,  7, 12,
     8, 23, 11,  5, 16, 26,
     1,  9, 19, 25,  4, 15,
    54, 43, 36, 29, 49, 40, 
    48, 30, 52, 44, 37, 33,
    46, 35, 50, 41, 28, 53,
    51, 55, 32, 45, 39, 42 };
#endif

static const uint8_t expansion_table[] = {
    31,  0,  1,  2,  3,  4,
     3,  4,  5,  6,  7,  8,
     7,  8,  9, 10, 11, 12,
    11, 12, 13, 14, 15, 16,
    15, 16, 17, 18, 19, 20,
    19, 20, 21, 22, 23, 24,
    23, 24, 25, 26, 27, 28,
    27, 28, 29, 30, 31,  0 };

void permutation( uint64_t input, uint64_t *output, const uint8_t table[], size_t num )
{
    *output &= 0x0000000000000000;

    for (uint8_t i = 0; i < num; i++ )
    {
        if (!!((1LL<<table[i]) & input))
        {
            //printf("set bit %d from bit %d\n", i, table[i]);
            *output |= (1LL<<i);
        }
    }
}

void rol_28_key(uint64_t *in_left, uint64_t *in_right, uint64_t *in_key)
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

    *in_key = (*in_left << 28) | *in_right;
}

void des_encrypt_block (uint64_t input, 
                  uint64_t output,
                  uint64_t iv,
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

    printf ( "iv: 0x%" PRIx64 "\n", iv);
    printf ( "input: 0x%" PRIx64 "\n", input);
    input ^= iv;
    printf ( "input_iv: 0x%" PRIx64 "\n", input);
    permutation(input, &output, initial_permutation_table, 64);
    printf ( "permuted: 0x%" PRIx64 "\n", output);

    printf ( "key: 0x%" PRIx64 "\n", key);
    permutation(key, &pc1key_left, pc1_table_left, 28);
    //printf ( "PC1 LEFT : 0x%" PRIx64 "\n", pc1key_left);
    permutation(key, &pc1key_right, pc1_table_right, 28);
    //printf ( "PC1 RIGHT: 0x%" PRIx64 "\n", pc1key_right);
    pc1key = (pc1key_left << 28) | pc1key_right;
    printf ( "PC1 KEY: 0x%" PRIx64 "\n", pc1key);
    
    uint64_t in_block = output&0x00000000FFFFFFFF;
    printf ( "LOWER INPUT: 0x%" PRIx64 "\n", in_block);
    for ( int round = 0; round < 16; round++)
    {
        printf ("ROUND [%d]\n", round);

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
    }
}

int main()
{
    //unsigned char key[] = "password";
    uint64_t key = 0x70617373776F7264;
    //unsigned char iv[] = "initialz";
    uint64_t iv = 0x696E697469616C7A;
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

    des_encrypt_block( input, output, iv, key ); 
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
