#ifndef __LATTICEZK_COMMON_HPP_
#define __LATTICEZK_COMMON_HPP_

// Common definitions

#define LATTICEZK_UNUSED(expr) { (void)(expr); }

#define LATTICEZK_ALIGNMENT 64

#if defined(__CUDACC__)
	#define LATTICEZK_ALIGN_DECLARATION(a, decl) __align__(a) decl
#elif defined(_MSC_VER)
	#define LATTICEZK_ALIGN_DECLARATION(a, decl) __declspec(align(a)) decl
#elif defined(__GNUC__)
	#define LATTICEZK_ALIGN_DECLARATION(a, decl) decl __attribute__((aligned(a)))
#else
	#error "unsupported C/C++ compiler"
#endif

#if defined(__CUDACC__) || defined(_MSC_VER)
	// Build must define _CRT_RAND_S and c++17 standard

	#define LATTICEZK_CLASS_STATIC_CONSTEXPR static const inline
	#define LATTICEZK_BEGIN_UNSIGNED_UNARY_MINUS \
__pragma(warning(push)) \
__pragma(warning(disable: 4146))
	#define LATTICEZK_END_UNSIGNED_UNARY_MINUS \
__pragma(warning(pop))
	#define LATTICEZK_M256I(v1, v2, v3, v4) _mm256_set_epi64x(v1, v2, v3, v4)
	#define LATTICEZK_M256D(v1, v2, v3, v4) _mm256_set_pd(v1, v2, v3, v4)
#else
	#define LATTICEZK_CLASS_STATIC_CONSTEXPR static constexpr
	#define LATTICEZK_BEGIN_UNSIGNED_UNARY_MINUS
	#define LATTICEZK_END_UNSIGNED_UNARY_MINUS
	#define LATTICEZK_M256I(v1, v2, v3, v4) {v1, v2, v3, v4}
	#define LATTICEZK_M256D(v1, v2, v3, v4) {v1, v2, v3, v4}
#endif

#ifdef _WIN32
	#define LATTICEZK_ALIGNED_ALLOC(a, size) _aligned_malloc(size, a)
	#define LATTICEZK_ALIGNED_FREE(p) _aligned_free(p)
#else
	#define LATTICEZK_ALIGNED_ALLOC(a, size) aligned_alloc(a, size)
	#define LATTICEZK_ALIGNED_FREE(p) free(p)
#endif

#endif // __LATTICEZK_COMMON_HPP_
