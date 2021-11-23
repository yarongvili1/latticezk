#ifndef __LATICEZK_CUDA_CUAVX_CUH_
#define __LATICEZK_CUDA_CUAVX_CUH_

// AVX functions and types are not supported by CUDA, so the ones needed are implemented here

#include <stdint.h>

namespace LatticeZK
{
	namespace Cuda
	{

#define LATTICEZK_DEVICE_UVFUNC(fname, vtype, vlen, vfield, op) \
	constexpr inline __device__ vtype fname(vtype a) \
	{ \
		vtype c = { 0 }; \
		for (int i = 0; i < vlen; i++) { \
			c.vfield[i] = op(a.vfield[i]); \
		} \
		return c; \
	}
#define LATTICEZK_DEVICE_BVFUNC(fname, vtype, vlen, vfield, op) \
	constexpr inline __device__ vtype fname(vtype a, vtype b) \
	{ \
		vtype c = { 0 }; \
		for (int i = 0; i < vlen; i++) { \
			c.vfield[i] = a.vfield[i] op b.vfield[i]; \
		} \
		return c; \
	}
#define LATTICEZK_DEVICE_BVFUNC2(fname, vtype, vlen, vfield, op1, op2) \
	constexpr inline __device__ vtype fname(vtype a, vtype b) \
	{ \
		vtype c = { 0 }; \
		for (int i = 0; i < vlen; i++) { \
			c.vfield[i] = (op2(a.vfield[i]) op1 b.vfield[i]); \
		} \
		return c; \
	}
#define LATTICEZK_DEVICE_BVFUNC3(fname, vtype, vlen, vfield, op1, op2) \
	constexpr inline __device__ vtype fname(vtype a, vtype b) \
	{ \
		vtype c = { 0 }; \
		for (int i = 0; i < vlen; i++) { \
			c.vfield[i] = (op2(a.vfield[i])) op1 (op2(b.vfield[i])); \
		} \
		return c; \
	}
#define LATTICEZK_DEVICE_CFUNC(fname, ctype, vtype) \
	constexpr inline __device__ ctype fname(vtype a) \
	{ \
		return *((ctype *)&a); \
	}
#define LATTICEZK_DEVICE_LFUNC(fname, vtype, vlen, vfield) \
	constexpr inline __device__ vtype fname(vtype const * m) \
	{ \
		vtype c = { 0 }; \
		for (int i = 0; i < vlen; i++) { \
			c.vfield[i] = m->vfield[i]; \
		} \
		return c; \
	}
#define LATTICEZK_DEVICE_SFUNC(fname, vtype, vlen, vfield) \
	constexpr inline __device__ void fname(vtype * m, vtype a) \
	{ \
		for (int i = 0; i < vlen; i++) { \
			m->vfield[i] = a.vfield[i]; \
		} \
	}
#define LATTICEZK_DEVICE_UCFUNC(fname, vtype, vlen, vfield, op, ctype) \
	constexpr inline __device__ vtype fname(vtype a, ctype b) \
	{ \
		vtype c = { 0 }; \
		for (int i = 0; i < vlen; i++) { \
			c.vfield[i] = a.vfield[i] op b; \
		} \
		return c; \
	}
#define LATTICEZK_DEVICE_EFUNC(fname, vtype, vfield, ctype) \
	constexpr inline __device__ vtype fname(ctype e3, ctype e2, ctype e1, ctype e0) \
	{ \
		vtype c = { 0 }; \
		c.vfield[0] = e0; \
		c.vfield[1] = e1; \
		c.vfield[2] = e2; \
		c.vfield[3] = e3; \
		return c; \
	}

typedef struct __align__(32) __m256d {
	double m256d_f64[4];
} __m256d;

typedef union __align__(32) __m256i {
	__int64             init_i64[4];
	__int8              m256i_i8[32];
	__int16             m256i_i16[16];
	__int32             m256i_i32[8];
	__int64             m256i_i64[4];
	unsigned __int8     m256i_u8[32];
	unsigned __int16    m256i_u16[16];
	unsigned __int32    m256i_u32[8];
	unsigned __int64    m256i_u64[4];
} __m256i;

LATTICEZK_DEVICE_BVFUNC(_mm256_add_epi64, __m256i, 4, m256i_i64, +);
LATTICEZK_DEVICE_BVFUNC(_mm256_add_pd, __m256d, 4, m256d_f64, +);
LATTICEZK_DEVICE_BVFUNC(_mm256_and_si256, __m256i, 4, m256i_i64, &);
LATTICEZK_DEVICE_CFUNC(_mm256_castpd_si256, __m256i, __m256d);
LATTICEZK_DEVICE_CFUNC(_mm256_castsi256_pd, __m256d, __m256i);
LATTICEZK_DEVICE_BVFUNC2(_mm256_cmpeq_epi64, __m256i, 4, m256i_i64, ==, -);
LATTICEZK_DEVICE_UVFUNC(_mm256_floor_pd, __m256d, 4, m256d_f64, floor);
LATTICEZK_DEVICE_LFUNC(_mm256_load_si256, __m256i, 4, m256i_i64);
LATTICEZK_DEVICE_LFUNC(_mm256_loadu_si256, __m256i, 4, m256i_i64);
LATTICEZK_DEVICE_BVFUNC3(_mm256_mul_epu32, __m256i, 4, m256i_u64, *, 0x00000000FFFFFFFFLL &);
LATTICEZK_DEVICE_BVFUNC(_mm256_mul_pd, __m256d, 4, m256d_f64, *);
LATTICEZK_DEVICE_BVFUNC(_mm256_or_si256, __m256i, 4, m256i_i64, |);
LATTICEZK_DEVICE_EFUNC(_mm256_set_epi64x, __m256i, m256i_i64, __int64);
LATTICEZK_DEVICE_EFUNC(_mm256_set_pd, __m256d, m256d_f64, double);
constexpr inline __device__ __m256i _mm256_setzero_si256()
{
	__m256i c = { 0, 0, 0, 0 };
	return c;
}
LATTICEZK_DEVICE_UCFUNC(_mm256_slli_epi64, __m256i, 4, m256i_u64, << , int);
LATTICEZK_DEVICE_BVFUNC(_mm256_sllv_epi64, __m256i, 4, m256i_u64, <<);
LATTICEZK_DEVICE_UCFUNC(_mm256_srli_epi64, __m256i, 4, m256i_u64, >>, int);
LATTICEZK_DEVICE_SFUNC(_mm256_store_si256, __m256i, 4, m256i_u64);
LATTICEZK_DEVICE_BVFUNC(_mm256_sub_epi64, __m256i, 4, m256i_u64, -);
LATTICEZK_DEVICE_BVFUNC(_mm256_sub_pd, __m256d, 4, m256d_f64, -);
constexpr inline __device__ __m256d _mm256_fmadd_pd(__m256d a, __m256d b, __m256d c)
{
	__m256d d = { 0, 0, 0, 0 };
	for (int i = 0; i < 4; i++) {
		// the following should be compiled into a FMA under full optimization
		d.m256d_f64[i] = (a.m256d_f64[i] * b.m256d_f64[i]) + c.m256d_f64[i];
	}
	return d;
}

	}
}

#endif // __LATICEZK_CUDA_CUAVX_CUH_