#ifndef XOSHIRO_H
#define XOSHIRO_H

#include <stdint.h>

void xoshiro_init(uint64_t *seed);
uint64_t xoshiro_next(void);

#endif /* XOSHIRO_H */