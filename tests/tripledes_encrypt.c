#include "common.h"
#include "des.h"
#include "fileio.h"

int main()
{
    uint64_t key_3des[3] = {0x7477656E7479666F,
                            0x7572636861726163,
                            0x746572696E707574};
    uint64_t salt = 0x696E697469616C7A;

    int infile = openinfile("./testfile.dat");
    int outfile = openoutfile("./enc.3des");

    tripledes_file(infile, outfile, key_3des, salt, ENCRYPT);

    closefile(infile);
    closefile(outfile);
}
