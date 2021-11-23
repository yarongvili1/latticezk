/* ****************************** *
 * Based on public domain code by *
 * Raymond K. ZHAO                *
 * gitlab.com/raykzhao/gaussian   *
 *                                *
 * Discrete Gaussian Sampler      *
 * ****************************** */

#include <stdint.h>
// gcem is used for compile-time computation of constants depending on
// the Gaussian's standard deviation parameter
#include "gcem.hpp"

// By default, this code compiles in CPU mode into namespace LatticeZK
// CUDA mode is enabled when LATTICEZK_GAUSSIAN_FACCT_CUDA is defined
// In CUDA mode, this code compiles into namespace LatticeZK::Cuda
// A Windows environment is assumed in CUDA mode
#ifdef LATTICEZK_GAUSSIAN_FACCT_CUDA
	#define LATTICEZK_CUDA_DEVICE __device__
	// pull-in CUDA implementations of AVX functions needed here
	#include "latticezk/cuda/cuavx.cuh"

	#define LATTICEZK_ALIGN_DECLARATION(a, decl) __align__(a) decl
	#define LATTICEZK_BEGIN_UNSIGNED_UNARY_MINUS \
		__pragma(warning(push)) \
		__pragma(warning(disable: 4146))
	#define LATTICEZK_END_UNSIGNED_UNARY_MINUS \
		__pragma(warning(pop))

	#define LATTICEZK_M256I(v1, v2, v3, v4) {v1, v2, v3, v4}
	#define LATTICEZK_M256D(v1, v2, v3, v4) {v1, v2, v3, v4}
	#define LATTICEZK_CLASS_STATIC_CONSTEXPR static constexpr

	#ifndef LATTICEZK_GAUSSIAN_FACCT_ALIGNMENT
		#define	LATTICEZK_GAUSSIAN_FACCT_ALIGNMENT sizeof(uint64_t)
	#endif
#else
	#define LATTICEZK_CUDA_DEVICE
	#include <immintrin.h>
#endif

namespace LatticeZK
{
#ifdef LATTICEZK_GAUSSIAN_FACCT_CUDA
	namespace Cuda
	{
#endif

#ifdef LATTICEZK_GAUSSIAN_FACCT_ALIGNMENT
		template<class T>
		static constexpr T facct_align_size(T x)
		{
			T a = (T)LATTICEZK_GAUSSIAN_FACCT_ALIGNMENT;
			return ((x + a - 1) / a) * a;
		}
#endif

LATTICEZK_CUDA_DEVICE static const int32_t CDT_ENTRY_SIZE = 16;
LATTICEZK_CUDA_DEVICE static const int64_t CDT_LOW_MASK = 0x7fffffffffffffff;
LATTICEZK_CUDA_DEVICE static const int32_t CDT_LENGTH = 9; /* [0..tau*sigma]=[0..9] */

LATTICEZK_CUDA_DEVICE static const int32_t BERNOULLI_ENTRY_SIZE = 9; /* 72bit randomness */

LATTICEZK_CUDA_DEVICE static const int64_t EXP_MANTISSA_PRECISION = 52;
LATTICEZK_CUDA_DEVICE static const int64_t EXP_MANTISSA_MASK = ((1LL << EXP_MANTISSA_PRECISION) - 1);
LATTICEZK_CUDA_DEVICE static const int64_t R_MANTISSA_PRECISION = (EXP_MANTISSA_PRECISION + 1);
LATTICEZK_CUDA_DEVICE static const int64_t R_MANTISSA_MASK = ((1LL << R_MANTISSA_PRECISION) - 1);
LATTICEZK_CUDA_DEVICE static const int64_t R_EXPONENT_L = (8 * BERNOULLI_ENTRY_SIZE - R_MANTISSA_PRECISION);

LATTICEZK_CUDA_DEVICE static const int64_t DOUBLE_ONE = (1023LL << 52);


LATTICEZK_CUDA_DEVICE static const int32_t BASE_TABLE_SIZE = (4 * CDT_ENTRY_SIZE);
LATTICEZK_CUDA_DEVICE static const int32_t BERNOULLI_TABLE_SIZE = (4 * BERNOULLI_ENTRY_SIZE);
#ifdef LATTICEZK_GAUSSIAN_FACCT_ALIGNMENT
LATTICEZK_CUDA_DEVICE static const int32_t BASE_TABLE_BLOCK_SIZE = facct_align_size(BASE_TABLE_SIZE);
LATTICEZK_CUDA_DEVICE static const int32_t BERNOULLI_TABLE_BLOCK_SIZE = facct_align_size(BERNOULLI_TABLE_SIZE);
#else
LATTICEZK_CUDA_DEVICE static const int32_t BASE_TABLE_BLOCK_SIZE = BASE_TABLE_SIZE;
LATTICEZK_CUDA_DEVICE static const int32_t BERNOULLI_TABLE_BLOCK_SIZE = BERNOULLI_TABLE_SIZE;
#endif

/* CDT table */
LATTICEZK_CUDA_DEVICE static const __m256i V_CDT[][2] = { {LATTICEZK_M256I(2200310400551559144LL, 2200310400551559144LL, 2200310400551559144LL, 2200310400551559144LL), LATTICEZK_M256I(3327841033070651387LL, 3327841033070651387LL, 3327841033070651387LL, 3327841033070651387LL)},
{LATTICEZK_M256I(7912151619254726620LL, 7912151619254726620LL, 7912151619254726620LL, 7912151619254726620LL), LATTICEZK_M256I(380075531178589176LL, 380075531178589176LL, 380075531178589176LL, 380075531178589176LL)},
{LATTICEZK_M256I(5167367257772081627LL, 5167367257772081627LL, 5167367257772081627LL, 5167367257772081627LL), LATTICEZK_M256I(11604843442081400LL, 11604843442081400LL, 11604843442081400LL, 11604843442081400LL)},
{LATTICEZK_M256I(5081592746475748971LL, 5081592746475748971LL, 5081592746475748971LL, 5081592746475748971LL), LATTICEZK_M256I(90134450315532LL, 90134450315532LL, 90134450315532LL, 90134450315532LL)},
{LATTICEZK_M256I(6522074513864805092LL, 6522074513864805092LL, 6522074513864805092LL, 6522074513864805092LL), LATTICEZK_M256I(175786317361LL, 175786317361LL, 175786317361LL, 175786317361LL)},
{LATTICEZK_M256I(2579734681240182346LL, 2579734681240182346LL, 2579734681240182346LL, 2579734681240182346LL), LATTICEZK_M256I(85801740LL, 85801740LL, 85801740LL, 85801740LL)},
{LATTICEZK_M256I(8175784047440310133LL, 8175784047440310133LL, 8175784047440310133LL, 8175784047440310133LL), LATTICEZK_M256I(10472LL, 10472LL, 10472LL, 10472LL)},
{LATTICEZK_M256I(2947787991558061753LL, 2947787991558061753LL, 2947787991558061753LL, 2947787991558061753LL), LATTICEZK_M256I(0LL, 0LL, 0LL, 0LL)},
{LATTICEZK_M256I(22489665999543LL, 22489665999543LL, 22489665999543LL, 22489665999543LL), LATTICEZK_M256I(0LL, 0LL, 0LL, 0LL)} };

LATTICEZK_CUDA_DEVICE static const __m256i V_CDT_LOW_MASK = LATTICEZK_M256I(CDT_LOW_MASK, CDT_LOW_MASK, CDT_LOW_MASK, CDT_LOW_MASK);

/* coefficients of the exp evaluation polynomial */
LATTICEZK_CUDA_DEVICE static const __m256i EXP_COFF[] = { LATTICEZK_M256I(0x3e833b70ffa2c5d4LL, 0x3e833b70ffa2c5d4LL, 0x3e833b70ffa2c5d4LL, 0x3e833b70ffa2c5d4LL),
									LATTICEZK_M256I(0x3eb4a480fda7e6e1LL, 0x3eb4a480fda7e6e1LL, 0x3eb4a480fda7e6e1LL, 0x3eb4a480fda7e6e1LL),
									LATTICEZK_M256I(0x3ef01b254493363fLL, 0x3ef01b254493363fLL, 0x3ef01b254493363fLL, 0x3ef01b254493363fLL),
									LATTICEZK_M256I(0x3f242e0e0aa273ccLL, 0x3f242e0e0aa273ccLL, 0x3f242e0e0aa273ccLL, 0x3f242e0e0aa273ccLL),
									LATTICEZK_M256I(0x3f55d8a2334ed31bLL, 0x3f55d8a2334ed31bLL, 0x3f55d8a2334ed31bLL, 0x3f55d8a2334ed31bLL),
									LATTICEZK_M256I(0x3f83b2aa56db0f1aLL, 0x3f83b2aa56db0f1aLL, 0x3f83b2aa56db0f1aLL, 0x3f83b2aa56db0f1aLL),
									LATTICEZK_M256I(0x3fac6b08e11fc57eLL, 0x3fac6b08e11fc57eLL, 0x3fac6b08e11fc57eLL, 0x3fac6b08e11fc57eLL),
									LATTICEZK_M256I(0x3fcebfbdff556072LL, 0x3fcebfbdff556072LL, 0x3fcebfbdff556072LL, 0x3fcebfbdff556072LL),
									LATTICEZK_M256I(0x3fe62e42fefa7fe6LL, 0x3fe62e42fefa7fe6LL, 0x3fe62e42fefa7fe6LL, 0x3fe62e42fefa7fe6LL),
									LATTICEZK_M256I(0x3ff0000000000000LL, 0x3ff0000000000000LL, 0x3ff0000000000000LL, 0x3ff0000000000000LL) };

LATTICEZK_CUDA_DEVICE static const __m256d V_INT64_DOUBLE = LATTICEZK_M256D(0x0010000000000000LL, 0x0010000000000000LL, 0x0010000000000000LL, 0x0010000000000000LL);
LATTICEZK_CUDA_DEVICE static const __m256d V_DOUBLE_INT64 = LATTICEZK_M256D(0x0018000000000000LL, 0x0018000000000000LL, 0x0018000000000000LL, 0x0018000000000000LL);

LATTICEZK_CUDA_DEVICE static const __m256i V_EXP_MANTISSA_MASK = LATTICEZK_M256I(EXP_MANTISSA_MASK, EXP_MANTISSA_MASK, EXP_MANTISSA_MASK, EXP_MANTISSA_MASK);
LATTICEZK_CUDA_DEVICE static const __m256i V_RES_MANTISSA = LATTICEZK_M256I(1LL << EXP_MANTISSA_PRECISION, 1LL << EXP_MANTISSA_PRECISION, 1LL << EXP_MANTISSA_PRECISION, 1LL << EXP_MANTISSA_PRECISION);
LATTICEZK_CUDA_DEVICE static const __m256i V_RES_EXPONENT = LATTICEZK_M256I(R_EXPONENT_L - 1023 + 1, R_EXPONENT_L - 1023 + 1, R_EXPONENT_L - 1023 + 1, R_EXPONENT_L - 1023 + 1);
LATTICEZK_CUDA_DEVICE static const __m256i V_R_MANTISSA_MASK = LATTICEZK_M256I(R_MANTISSA_MASK, R_MANTISSA_MASK, R_MANTISSA_MASK, R_MANTISSA_MASK);
LATTICEZK_CUDA_DEVICE static const __m256i V_1 = LATTICEZK_M256I(1, 1, 1, 1);
LATTICEZK_CUDA_DEVICE static const __m256i V_DOUBLE_ONE = LATTICEZK_M256I(DOUBLE_ONE, DOUBLE_ONE, DOUBLE_ONE, DOUBLE_ONE);

/* constant time CDT sampler */
LATTICEZK_CUDA_DEVICE static inline __m256i cdt_sampler(unsigned char* r)
{
	__m256i x = _mm256_setzero_si256();
	__m256i r1, r2;
	__m256i r1_lt_cdt0, r2_lt_cdt1;
	__m256i r2_eq_cdt1;
	__m256i b;

	uint32_t i;

	r1 = _mm256_loadu_si256((__m256i*)r);
	r2 = _mm256_loadu_si256((__m256i*)(r + 32));

	r1 = _mm256_and_si256(r1, V_CDT_LOW_MASK);
	r2 = _mm256_and_si256(r2, V_CDT_LOW_MASK);

	for (i = 0; i < CDT_LENGTH; i++)
	{
		r1_lt_cdt0 = _mm256_sub_epi64(r1, V_CDT[i][0]);

		r2_lt_cdt1 = _mm256_sub_epi64(r2, V_CDT[i][1]);
		r2_eq_cdt1 = _mm256_cmpeq_epi64(r2, V_CDT[i][1]);

		b = _mm256_and_si256(r1_lt_cdt0, r2_eq_cdt1);
		b = _mm256_or_si256(b, r2_lt_cdt1);
		b = _mm256_srli_epi64(b, 63);

		x = _mm256_add_epi64(x, b);
	}

	return x;
}


static constexpr uint8_t facct_size_of(int64_t x) {
	return x < (1LL << 8) ? 1 : x < (1LL << 16) ? 2 : x < (1LL << 32) ? 4 : 8;
}

namespace {

// only 8-, 16-, and 32-bit-wide sigma values are supported
// 64-bit-wide sigma values are hard to justify and would
// require handling overflow in various places in the code
template<uint8_t S> struct Uniform;
template<> struct Uniform<1> { typedef uint8_t  type; };
template<> struct Uniform<2> { typedef uint16_t type; };
template<> struct Uniform<4> { typedef uint32_t type; };

template<class BytesSampler, uint32_t Sigma>
class BatchFacctGaussianSampler
{
private:
	BytesSampler& bytes_sampler;
public:
	LATTICEZK_CUDA_DEVICE BatchFacctGaussianSampler(BytesSampler& bytes_sampler) :
		bytes_sampler(bytes_sampler)
	{
	}
public:

static constexpr double sigma_0 = gcem::sqrt(1.0 / (2.0 * gcem::log(2)));
static constexpr double inv_sigma_0 = 1.0 / sigma_0;
static constexpr double sigma = Sigma;

/* the closest integer k such that k*sigma_0=sigma */
static constexpr int64_t BINARY_SAMPLER_K = (int64_t)gcem::ceil(sigma * inv_sigma_0);

/* -1/k^2 */
static constexpr double BINARY_SAMPLER_K_2_INV = ((-1.0 / BINARY_SAMPLER_K) / BINARY_SAMPLER_K);

static constexpr uint8_t UNIFORM_SIZE = facct_size_of(BINARY_SAMPLER_K);
static constexpr int BARRETT_BITSHIFT = (UNIFORM_SIZE * 8);
static constexpr uint64_t BARRETT_FACTOR = ((1LL << BARRETT_BITSHIFT) / BINARY_SAMPLER_K);
static constexpr uint64_t UNIFORM_Q = (BINARY_SAMPLER_K * BARRETT_FACTOR);
static constexpr uint64_t UNIFORM_CHOICES = 1LL << BARRETT_BITSHIFT;
static constexpr double UNIFORM_REJ_PROB = (1.0 * (UNIFORM_CHOICES - UNIFORM_Q)) / UNIFORM_CHOICES;
static constexpr int32_t UNIFORM_REJ_TARGET_PROB_BITS = 64;
static constexpr int32_t UNIFORM_REJ = (int32_t)gcem::ceil(UNIFORM_REJ_TARGET_PROB_BITS * gcem::log(0.5) / gcem::log(UNIFORM_REJ_PROB));

static constexpr int32_t TABLES_SAMPLE_BYTES = 2 * (BASE_TABLE_SIZE + BERNOULLI_TABLE_SIZE);
static constexpr int32_t UNIFORM_SAMPLE_BYTES = UNIFORM_REJ * UNIFORM_SIZE;
static constexpr int32_t EXTRA_SAMPLE_BYTES = 1;
#ifdef LATTICEZK_GAUSSIAN_FACCT_ALIGNMENT
// In CUDA mode we ensure each block is 8-byte-aligned to avoid runtime errors
static constexpr int32_t TABLES_SAMPLE_BLOCK_BYTES = facct_align_size(TABLES_SAMPLE_BYTES);
static constexpr int32_t UNIFORM_SAMPLE_BLOCK_BYTES = facct_align_size(UNIFORM_SAMPLE_BYTES);
static constexpr int32_t EXTRA_SAMPLE_BLOCK_BYTES = facct_align_size(EXTRA_SAMPLE_BYTES);
#else
static constexpr int32_t TABLES_SAMPLE_BLOCK_BYTES = TABLES_SAMPLE_BYTES;
static constexpr int32_t UNIFORM_SAMPLE_BLOCK_BYTES = UNIFORM_SAMPLE_BYTES;
static constexpr int32_t EXTRA_SAMPLE_BLOCK_BYTES = EXTRA_SAMPLE_BYTES;
#endif
static constexpr int32_t ALL_SAMPLE_BLOCK_BYTES = TABLES_SAMPLE_BLOCK_BYTES + UNIFORM_SAMPLE_BLOCK_BYTES + EXTRA_SAMPLE_BLOCK_BYTES;

LATTICEZK_CLASS_STATIC_CONSTEXPR __m256i V_K_K_K_K = LATTICEZK_M256I(BINARY_SAMPLER_K, BINARY_SAMPLER_K, BINARY_SAMPLER_K, BINARY_SAMPLER_K);

LATTICEZK_CLASS_STATIC_CONSTEXPR __m256d V_K_2_INV = LATTICEZK_M256D(BINARY_SAMPLER_K_2_INV, BINARY_SAMPLER_K_2_INV, BINARY_SAMPLER_K_2_INV, BINARY_SAMPLER_K_2_INV);

/* constant time Bernoulli sampler
 * we directly compute exp(-x/(2*sigma_0^2)),
 * since sigma_0=sqrt(1/2ln2), exp(-x/(2*sigma_0^2))=2^(-x/k^2),
 * we use a polynomial to directly evaluate 2^(-x/k^2) */
LATTICEZK_CUDA_DEVICE static inline void bernoulli_sampler(uint64_t* b, __m256i x, unsigned char* r)
{
	__m256d vx, vx_1, vx_2, vsum;
	__m256i vt, k, vres, vres_mantissa, vres_exponent, vr_mantissa, vr_exponent, vr_exponent2, vres_eq_1, vr_lt_vres_mantissa, vr_lt_vres_exponent;

	/* 2^x=2^(floor(x)+a)=2^(floor(x))*2^a, where a is in [0,1]
	 * we only evaluate 2^a by using a polynomial */
	x = _mm256_or_si256(x, _mm256_castpd_si256(V_INT64_DOUBLE));
	vx = _mm256_sub_pd(_mm256_castsi256_pd(x), V_INT64_DOUBLE);
	vx = _mm256_mul_pd(vx, V_K_2_INV);

	vx_1 = _mm256_floor_pd(vx);
	vx_2 = _mm256_add_pd(vx_1, V_DOUBLE_INT64);
	vt = _mm256_sub_epi64(_mm256_castpd_si256(vx_2), _mm256_castpd_si256(V_DOUBLE_INT64));
	vt = _mm256_slli_epi64(vt, 52);

	/* evaluate 2^a */
	vx_2 = _mm256_sub_pd(vx, vx_1);
	vsum = _mm256_fmadd_pd(_mm256_castsi256_pd(EXP_COFF[0]), vx_2, _mm256_castsi256_pd(EXP_COFF[1]));
	vsum = _mm256_fmadd_pd(vsum, vx_2, _mm256_castsi256_pd(EXP_COFF[2]));
	vsum = _mm256_fmadd_pd(vsum, vx_2, _mm256_castsi256_pd(EXP_COFF[3]));
	vsum = _mm256_fmadd_pd(vsum, vx_2, _mm256_castsi256_pd(EXP_COFF[4]));
	vsum = _mm256_fmadd_pd(vsum, vx_2, _mm256_castsi256_pd(EXP_COFF[5]));
	vsum = _mm256_fmadd_pd(vsum, vx_2, _mm256_castsi256_pd(EXP_COFF[6]));
	vsum = _mm256_fmadd_pd(vsum, vx_2, _mm256_castsi256_pd(EXP_COFF[7]));
	vsum = _mm256_fmadd_pd(vsum, vx_2, _mm256_castsi256_pd(EXP_COFF[8]));
	vsum = _mm256_fmadd_pd(vsum, vx_2, _mm256_castsi256_pd(EXP_COFF[9]));

	/* combine to compute 2^x */
	vres = _mm256_add_epi64(vt, _mm256_castpd_si256(vsum));

	/* compute the Bernoulli value */
	vres_mantissa = _mm256_and_si256(vres, V_EXP_MANTISSA_MASK);
	vres_mantissa = _mm256_or_si256(vres_mantissa, V_RES_MANTISSA);

	vres_exponent = _mm256_srli_epi64(vres, EXP_MANTISSA_PRECISION);
	vres_exponent = _mm256_add_epi64(vres_exponent, V_RES_EXPONENT);
	vres_exponent = _mm256_sllv_epi64(V_1, vres_exponent);

	vr_mantissa = _mm256_loadu_si256((__m256i*)r);
	vr_exponent = _mm256_srli_epi64(vr_mantissa, R_MANTISSA_PRECISION);
	vr_mantissa = _mm256_and_si256(vr_mantissa, V_R_MANTISSA_MASK);
	vr_exponent2 = _mm256_set_epi64x(r[35], r[34], r[33], r[32]);
	vr_exponent2 = _mm256_slli_epi64(vr_exponent2, 64 - R_MANTISSA_PRECISION);
	vr_exponent = _mm256_or_si256(vr_exponent, vr_exponent2);

	/* (res == 1.0) || ((r_mantissa < res_mantissa) && (r_exponent < (1 << res_exponent))) */
	vres_eq_1 = _mm256_cmpeq_epi64(vres, V_DOUBLE_ONE);
	vr_lt_vres_mantissa = _mm256_sub_epi64(vr_mantissa, vres_mantissa);
	vr_lt_vres_exponent = _mm256_sub_epi64(vr_exponent, vres_exponent);

	k = _mm256_and_si256(vr_lt_vres_mantissa, vr_lt_vres_exponent);
	k = _mm256_or_si256(k, vres_eq_1);

	_mm256_store_si256((__m256i*)(b), k);
}

/* make sure that Pr(rerun the PRG)<=2^(-64) */
LATTICEZK_CUDA_DEVICE static inline void uniform_sampler(unsigned char* r, __m256i y[2])
{
	uint64_t * sample = (uint64_t *)y;
	uint32_t i = 0, j = 0;
	uint64_t x = 0;

	while (j < 8)
	{
		do
		{	/* we ignore the low probability of rerunning the PRG */
			x = ((typename Uniform<UNIFORM_SIZE>::type*)r)[i++];
		} while (1 ^ ((x - UNIFORM_Q) >> 63));

		x = x - (((((int64_t)x * BARRETT_FACTOR) >> BARRETT_BITSHIFT) + 1) * BINARY_SAMPLER_K);
		x = x + (x >> 63) * BINARY_SAMPLER_K;

		sample[j++] = x;
	}
}

/* binary sampling algorithm
 * we compute 8 samples every time by using the AVX2,
 * then do the rejection */
LATTICEZK_CUDA_DEVICE uint32_t sample(int64_t* sample, uint32_t slen)
{
	LATTICEZK_ALIGN_DECLARATION(32, uint64_t z[8]);
	LATTICEZK_ALIGN_DECLARATION(32, uint64_t b[8]);

#ifdef LATTICEZK_GAUSSIAN_FACCT_CUDA
	unsigned char* r0 = nullptr;
#else
	LATTICEZK_ALIGN_DECLARATION(32, unsigned char r0[ALL_SAMPLE_BLOCK_BYTES]);
#endif
	unsigned char * r; // this gets set to r0 in CPU mode only

	uint32_t i = 8, j = 0;
	uint64_t k;

	while (j < slen)
	{
		do
		{
			if (i == 8)
			{
				if (nullptr == (r = sample_round(z, b, r0))) {
					return j;
				}
				i = 0;
			}

			k = (r[ALL_SAMPLE_BLOCK_BYTES - EXTRA_SAMPLE_BLOCK_BYTES] >> i) & 0x1;
			i++;
			LATTICEZK_BEGIN_UNSIGNED_UNARY_MINUS
		} while (1 ^ ((b[i - 1] & ((z[i - 1] | -z[i - 1]) | (k | -k))) >> 63)); /* rejection condition: b=0 or ((b=1) && (z=0) && (k=0)) */

			sample[j++] = z[i - 1] * (1 ^ ((-k) & 0xfffffffffffffffe)); /* sample=z*(-1)^k */
			LATTICEZK_END_UNSIGNED_UNARY_MINUS
	}
	return j;
}

private:
LATTICEZK_CUDA_DEVICE unsigned char * sample_round(uint64_t * z, uint64_t * b, unsigned char * r)
{
	// r is modified in CUDA mode only, and returned in both CUDA and CPU modes
	__m256i v_x, v_y[2], v_z, v_b_in;
	unsigned char *r1;

	/* x<--D_sigma_0, y<--U([0,k-1]), z=kx+y */
	if (nullptr == (r = bytes_sampler(r, ALL_SAMPLE_BLOCK_BYTES))) {
		// ran out of (pre-generated) randomness - this can only happen in CUDA mode
		return nullptr;
	}

	uniform_sampler(r + TABLES_SAMPLE_BLOCK_BYTES, v_y);

	r1 = r;
	v_x = cdt_sampler(r1);
	v_x = _mm256_mul_epu32(v_x, V_K_K_K_K);
	v_z = _mm256_add_epi64(v_x, v_y[0]);
	_mm256_store_si256((__m256i*)(z), v_z);
	/* b<--Bernoulli(exp(-y(y+2kx)/2sigma_0^2)) */
	v_b_in = _mm256_add_epi64(v_z, v_x);
	v_b_in = _mm256_mul_epu32(v_b_in, v_y[0]);
	bernoulli_sampler(b, v_b_in, r1 + BASE_TABLE_BLOCK_SIZE);

	r1 = r + (BASE_TABLE_BLOCK_SIZE + BERNOULLI_TABLE_BLOCK_SIZE);
	v_x = cdt_sampler(r1);
	v_x = _mm256_mul_epu32(v_x, V_K_K_K_K);
	v_z = _mm256_add_epi64(v_x, v_y[1]);
	_mm256_store_si256((__m256i*)(z + 4), v_z);
	/* b<--Bernoulli(exp(-y(y+2kx)/2sigma_0^2)) */
	v_b_in = _mm256_add_epi64(v_z, v_x);
	v_b_in = _mm256_mul_epu32(v_b_in, v_y[1]);
	bernoulli_sampler(b + 4, v_b_in, r1 + BASE_TABLE_BLOCK_SIZE);

	return r;
}
};

} // anonymous namespace

template<uint32_t Sigma, int nsamples = 256, class BSampler = BytesSampler>
class FacctGaussianSampler
{
public:
	typedef BatchFacctGaussianSampler<BSampler, Sigma> GSampler;
public:
	static constexpr double sigma = Sigma;
	static constexpr int BitsPerSample = (int)gcem::ceil(gcem::log(sigma) / gcem::log(2)) + 1 + 2; // 4*sigma in bits
	static constexpr int NSAMPLES = nsamples;
private:
	BSampler& bytes_sampler;
	GSampler gsampler;
	int64_t samples[NSAMPLES] = { 0 };
	int cursor, nsampled;
private:
	LATTICEZK_CUDA_DEVICE inline void fill()
	{
		nsampled = gsampler.sample(samples, NSAMPLES);
		cursor = 0;
	}
public:
	LATTICEZK_CUDA_DEVICE FacctGaussianSampler(BSampler & bytes_sampler) :
		bytes_sampler(bytes_sampler), gsampler(bytes_sampler)
	{
		cursor = nsampled = 0;
	}
public:
	LATTICEZK_CUDA_DEVICE inline int64_t operator()()
	{
		if (cursor == nsampled) fill();
		return samples[cursor++];
	}
	LATTICEZK_CUDA_DEVICE inline uint64_t get_rejections() const {
		return 0;
	}
};

#ifdef LATTICEZK_GAUSSIAN_FACCT_CUDA
	} // namespace Cuda
#endif
} // namespace LatticeZK

#ifdef LATTICEZK_GAUSSIAN_FACCT_CUDA
	#undef LATTICEZK_CUDA_DEVICE
	#undef LATTICEZK_ALIGN_DECLARATION
	#undef LATTICEZK_BEGIN_UNSIGNED_UNARY_MINUS
	#undef LATTICEZK_END_UNSIGNED_UNARY_MINUS
	#undef LATTICEZK_M256I
	#undef LATTICEZK_M256D
	#undef LATTICEZK_CLASS_STATIC_CONSTEXPR
	#undef LATTICEZK_GAUSSIAN_FACCT_ALIGNMENT
#else
	#undef LATTICEZK_CUDA_DEVICE
#endif