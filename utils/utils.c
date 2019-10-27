#include "utils.h"
bool get_bit(const uint64_t *data, uint8_t bit)
{
    return !!(*data & (1<<bit));
}

void set_bit(uint64_t *data, bit)
{
    *data |= (1<<bit);
}

void clear_bit(uint64_t *data, bit)
{
    *data &= (1<<bit);
}

#if 0
int main()
{
    uint64_t a = 0xffffffffffffffff;
    uint64_t b = 0x0000000000000000;
    uint64_t c = 0xaaaaaaaaaaaaaaaa;

    printf("=========================\n");
    for ( int i = 0; i < 64; i++ )
        printf ( "bit [%s]\n", get_bit(&a,i)?"ON":"OFF");
    printf("=========================\n");
    for ( int i = 0; i < 64; i++ )
        printf ( "bit [%s]\n", get_bit(&b,i)?"ON":"OFF");
    printf("=========================\n");
    for ( int i = 0; i < 64; i++ )
        printf ( "bit [%s]\n", get_bit(&c,i)?"ON":"OFF");
    printf("=========================\n");

    return 0;
}
#endif
