#ifndef __UTILS_H_
#define __UTILS_H_

#include "common.h"

bool get_bit(const uint64_t *data, uint8_t bit);
void set_bit(uint64_t *data, uint8_t bit);
void clear_bit(uint64_t *data, uint8_t bit);

#endif // __UTILS_H_
