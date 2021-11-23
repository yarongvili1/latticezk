#ifndef __LATTICEZK_TESTCOMMON_H_
#define __LATTICEZK_TESTCOMMON_H_

//! \brief Measure RDTSC at start of segment
//! \returns the RDTSC measurement
inline uint64_t rdtsc_start() {
	uint32_t low, high;
	__asm__ __volatile__ (
		"xorl %%eax,%%eax \n    cpuid"
		::: "%rax", "%rbx", "%rcx", "%rdx" );
	__asm__ __volatile__ (
		"lfence;rdtsc" : "=a" (low), "=d" (high));
		return (uint64_t)high << 32 | low;
}

//! \brief Measure RDTSC at end of segment
//! \returns the RDTSC measurement
inline uint64_t rdtsc_stop() {
	uint32_t low, high;
	__asm__ __volatile__ (
		"lfence;rdtsc" : "=a" (low), "=d" (high));
	__asm__ __volatile__ (
		"xorl %%eax,%%eax \n    cpuid"
		::: "%rax", "%rbx", "%rcx", "%rdx" );
		return (uint64_t)high << 32 | low;
}

#endif /* __LATTICEZK_TESTCOMMON_H_ */
