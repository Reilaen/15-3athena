// Copyright (c) Athena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

#include "../common/showmsg.h"
#include "../common/timer.h"
#include "random.h"

#if defined(_WIN32)
	#include <windows.h>
	#include <bcrypt.h>
	#pragma comment(lib, "bcrypt.lib") // Sagt dem Windows-Linker, dass er die Bibliothek braucht
#elif defined(__linux__)
	#include <sys/random.h>
	#include <unistd.h>
	#include <time.h>
	#include <string.h>
#else
	#include <stdlib.h>
#endif
#include <xoshiro.h>

static uint64 splitmix64(uint64 *x) {
	uint64 z = (*x += 0x9e3779b97f4a7c15ULL);
	z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
	z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;

	return z ^ (z >> 31);
}

void rnd_init(void) {
	uint64 s[4] = {0};

#if defined(_WIN32)
	BCryptGenRandom(NULL, (PUCHAR)s, sizeof(s), BCRYPT_USE_SYSTEM_PREFERRED_RNG);
#elif defined(__linux__)
	if (getrandom(s, sizeof(s), 0) != (ssize_t)sizeof(s)) {
		memset(s, 0, sizeof(s));
	}
#else
	arc4random_buf(s, sizeof(s));
#endif

	if ((s[0] | s[1] | s[2] | s[3]) == 0) {
		struct timespec ts;
		clock_gettime(CLOCK_MONOTONIC, &ts);

		uint64 sm_seed = (uint64)time(NULL);
		sm_seed ^= (uint64)ts.tv_nsec << 32;
		sm_seed ^= (uint64)getpid() << 16;
		sm_seed ^= (uintptr_t)&s;

		s[0] = splitmix64(&sm_seed);
		s[1] = splitmix64(&sm_seed);
		s[2] = splitmix64(&sm_seed);
		s[3] = splitmix64(&sm_seed);
	}

	xoshiro_init(s);

	ShowInfo("Initialized xoshiro256** with 256-bit system entropy.\n");
}


static inline uint32 rnd_uint32_core(const uint32 range) {
	uint32 x = rnd_uint32();
	uint64 m = (uint64)x * (uint64)range;
	uint32 l = (uint32)m;

	if (l < range) {
		const uint32_t t = -range % range;

		while (l < t) {
			x = rnd_uint32();
			m = (uint64)x * (uint64)range;
			l = (uint32)m;
		}
	}

	return (uint32)(m >> 32);
}

/// Generates a random number in the interval [0, UINT32_MAX]
uint32 rnd_uint32(void) {
	return (uint32)(xoshiro_next() >> 32);
}

/// Generates a random number in the interval [min, max]
/// Returns min if range is invalid.
uint32 rnd_value_uint32(const uint32 min, const uint32 max) {
	if (min >= max)
		return min;

	return min + rnd_uint32_core(max - min + 1);
}

int32 rnd_value_int32(const int32 min, const int32 max) {
	if (min >= max)
		return min;

	return min + (int32)rnd_uint32_core((uint32)max - (uint32)min + 1);
}

/// Check if we rolled a number <= chance
bool rnd_chance(const uint32 chance, const uint32 base) {
	if (chance == 0 || base == 0)
		return false;

	if (base <= chance)
		return true;

	return rnd_uint32_core(base) < chance;
}
