// Copyright (c) Athena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

#include "../common/showmsg.h"
#include "../common/timer.h" // gettick
#include "random.h"

#if defined(_WIN32)
	#include <windows.h>
	#include <bcrypt.h>
	#pragma comment(lib, "bcrypt.lib") // Sagt dem Windows-Linker, dass er die Bibliothek braucht
#elif defined(__linux__)
	#include <sys/random.h>
#else
	#include <stdlib.h>
#endif
#include <mt19937ar.h> // init_genrand, genrand_int32, genrand_res53

void rnd_init(void)
{
	unsigned long seed = 0;

	#if defined(_WIN32)
		BCryptGenRandom(NULL, (PUCHAR)&seed, sizeof(seed), BCRYPT_USE_SYSTEM_PREFERRED_RNG);
	#elif defined(__linux__)
		getrandom(&seed, sizeof(seed), 0);
	#else
		seed = arc4random();
	#endif

	ShowInfo("Initializing random number generator.\n");

	init_genrand(seed);
}

/// Generates a random number in the interval [0, UINT32_MAX]
uint32 rnd_uint32() {
	return (uint32)genrand_int32();
}

/// Generates a random number in the interval [min, max]
/// Returns min if range is invalid.
int32 rnd_value_int32(int32 min, int32 max) {
	if(min >= max)
		return min;

	return min + (int32)(genrand_real2() * ((double)max - (double)min + 1.0));
}

uint32 rnd_value_uint32(uint32 min, uint32 max) {
	if(min >= max)
		return min;

	return min + (uint32)(genrand_real2() * ((double)max - (double)min + 1.0));
}

int64 rnd_value_int64(int64 min, int64 max) {
	if(min >= max)
		return min;

	return min + (int64)(genrand_res53() * ((double)max - (double)min + 1.0));
}

bool rnd_chance(uint32 chance, uint32 base) {
	if(chance == 0 || base == 0)
		return false;

	if(chance >= base)
		return true;

	return rnd_value_uint32(1, base) <= chance;
}
