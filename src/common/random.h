// Copyright (c) Athena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

#ifndef _RANDOM_H_
#define _RANDOM_H_

void rnd_init(void);
uint32 rnd_uint32(void);// [0, UINT32_MAX]
int32 rnd_value_int32(int32 min, int32 max);// [min, max]
uint32 rnd_value_uint32(uint32 min, uint32 max);// [min, max]
bool rnd_chance(uint32 chance, uint32 base);

#endif /* _RANDOM_H_ */
