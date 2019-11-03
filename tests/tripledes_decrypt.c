#include "common.h"
#include "des.h"
#include "fileio.h"

int main()
{
    //unsigned char key[] = "password";
    //unsigned char key_3des[] = "twentyfourcharacterinput";
    //unsigned char salt[] = "initialz";
    //unsigned char input[] = "abcdefgh";

    int infile = openinfile("./testfile.txt");
    int outfile = openoutfile("./output.bin");

    uint64_t key = 0x70617373776F7264;
    uint64_t key_3des[3] = {0x7477656E7479666F,
                            0x7572636861726163,
                            0x746572696E707574};
    /* 7477656E7479666F7572636861726163746572696E707574 */
    uint64_t salt = 0x696E697469616C7A;

    uint64_t output[5];
    uint64_t output2[5];


    des (input,output,5,key,salt,ENCRYPT);
    for ( int i = 0; i < 5; i++)
        printf ( "DES: 0x%" PRIx64 "\n", output[i]);

    des (output,output2,5,key,salt,DECRYPT);
    for ( int i = 0; i < 5; i++)
        printf ( "PT: 0x%" PRIx64 "\n", output2[i]);

    tripledes (input,output,5,key_3des,salt,ENCRYPT);
    for ( int i = 0; i < 5; i++)
        printf ( "3DES: 0x%" PRIx64 "\n", output[i]);

    tripledes (output,output2,5,key_3des,salt,DECRYPT);
    for ( int i = 0; i < 5; i++)
        printf ( "PT: 0x%" PRIx64 "\n", output2[i]);
}

