// Copyright (c) Athena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

#ifndef _RANDOM_H_
#define _RANDOM_H_

#include "../common/cbasetypes.h"

void rnd_init(void);
uint32 rnd_uint32(void);// [0, UINT32_MAX]
int32 rnd_value_int32(int32 min, int32 max);// [min, max]
uint32 rnd_value_uint32(uint32 min, uint32 max);// [min, max]
double rnd_uniform(void);// [0.0, 1.0)
bool rnd_chance(uint32 chance, uint32 base);
int64 rnd_value_int64(int64 min, int64 max);

#endif /* _RANDOM_H_ */
