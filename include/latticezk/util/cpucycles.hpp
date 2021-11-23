#ifndef __LATTICEZK_UTIL_CPUCYCLES_HPP_
#define __LATTICEZK_UTIL_CPUCYCLES_HPP_

#include <inttypes.h>

#ifdef _WIN32
	#include <intrin.h>
#else
	#include <x86intrin.h>
#endif

inline uint64_t cpucycles()
{
	return __rdtsc();
}

#endif // __LATTICEZK_UTIL_CPUCYCLES_HPP_
