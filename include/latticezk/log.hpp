#ifndef __LATTICEZK_LOG_HPP_
#define __LATTICEZK_LOG_HPP_

// Simple logging facilities

#define LATTICEZK_LOG_ENABLED 1

#if LATTICEZK_LOG_ENABLED != 0
	#include <iostream>

	#define LATTICEZK_LOG(a) std::cerr << a << std::endl;
	#define LATTICEZK_TIMER_START(what) { auto _w = what; uint64_t _t0 = cpucycles();
	#define LATTICEZK_TIMER_END uint64_t _t1 = cpucycles(); LATTICEZK_LOG(_w << " : cycles=" << (_t1 - _t0)); }
#else
	#define LATTICEZK_LOG(a)
	#define LATTICEZK_TIMER_START(what) (what)
	#define LATTICEZK_TIMER_END
#endif

#define LATTICEZK_TIME(success, expr, msg) \
	do { \
		LATTICEZK_TIMER_START(msg); \
		success = success && (!!(expr)); \
		LATTICEZK_TIMER_END; \
		if (!success) { \
			LATTICEZK_LOG("FAILED : " << msg); \
		} \
	} while (0);


#endif // __LATTICEZK_LOG_HPP_
