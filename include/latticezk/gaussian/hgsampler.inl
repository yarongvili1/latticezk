// Code-generation for half-Gaussian sampler
// The following macros must be defined before inclusion of this file:
//   - HGSAMPLER_CLASS_SUFFIX : a suffix for the class name with prefix "HalfGaussianSampler" to be generated
//   - HGSAMPLER_INCLUDE : pre-generated Boolean logic for the half-sampler (see fork of const_gauss_split repo)
//   - HGSAMPLER_SIGMA : mean of the half-Gaussian sampler, as set for the pre-generated Boolean logic
//   - HGSAMPLER_N_OUT : number of output bits of the half-Gaussian sampler, as set for the pre-generated Boolean logic
//
// This file can be included more than once to generate multiple half-Gaussian samplers
// The generated class name (due to HGSAMPLER_CLASS_SUFFIX or a namespace) must be different each time
//
// The generated half-Guassian sampler code uses AVX if enabled at compile-time

#if !defined(HGSAMPLER_CLASS_SUFFIX) || !defined(HGSAMPLER_INCLUDE) || !defined(HGSAMPLER_SIGMA) || !defined(HGSAMPLER_N_OUT)
	#error "HGSAMPLER_CLASS_SUFFIX, HGSAMPLER_INCLUDE and HGSAMPLER_SIGMA and HGSAMPLER_N_OUT macros must be defined"
#endif

#define BITS_PER_SAMPLE HGSAMPLER_N_OUT
#define HGSAMPLER_CONCAT(x,y) x##y
#define HGSAMPLER_CONCAT2(x,y) HGSAMPLER_CONCAT(x,y)
#define HGSAMPLER_CLASS_NAME HGSAMPLER_CONCAT2(HalfGaussianSampler,HGSAMPLER_CLASS_SUFFIX())

#ifdef __AVX__
	#include <immintrin.h>
#endif

class HGSAMPLER_CLASS_NAME
{
private:

#ifdef __AVX__
	#define ALIGNMENT 32
	#define ALIGN __attribute__ ((aligned(ALIGNMENT)))
	#if HGSAMPLER_N_OUT <= 8
		typedef uint8_t Bsingle;
		typedef int8_t Bvec __attribute__ ((vector_size(ALIGNMENT))) ALIGN;
	        #define BCONST(c) {c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c}
	#elif HGSAMPLER_N_OUT <= 16
		typedef uint16_t Bsingle;
		typedef int16_t Bvec __attribute__ ((vector_size(ALIGNMENT))) ALIGN;
	        #define BCONST(c) {c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c}
	#else
		#error "no support for bits-per-sampler over 16"
	#endif
	#define BZERO BCONST(0)
	#define BONES BCONST(-1)
	#define BMASK BCONST(1)
	#define BTEST_NOT_ONES(x) (!_mm256_testz_si256(~((__m256i)x),~((__m256i)x)))
#else
	#define ALIGNMENT 0
	#define ALIGN
	typedef uint64_t Bsingle;
	typedef uint64_t Bvec;
	#define BZERO 0
	#define BONES 0xffffffffffffffff
	#define BMASK 0x0101010101010101
	#define BTEST_NOT_ONES(x) (x!=BONES)
#endif

public:
	static constexpr int NSAMPLES = ((int)(sizeof(Bvec)*8));
	static constexpr double Sigma = HGSAMPLER_SIGMA;
	static constexpr int BitsPerSample = BITS_PER_SAMPLE;
private:
	AES_Random & aes_rnd;
private:
	Bvec bit[128];// to hold the bits
	Bvec out[BITS_PER_SAMPLE], out_t[BITS_PER_SAMPLE];
	int sample[NSAMPLES];
	int cursor;
private:
	Bsingle sample_o[NSAMPLES] ALIGN;
 	Bvec *sample_n = (Bvec*) ((void*) sample_o);
	Bvec sample_t;
	Bvec mask = BMASK;
	Bvec bzero = BZERO;
	Bvec bones = BONES;
	long int nrotate = sizeof(Bsingle)*8;
private:
void fill()
{
	long int j,k;
	Bvec disable_update,control;
	
	for(j=0;j<(int)sizeof(bit);j+=16){
		aes_rnd.random_bytes(((uint8_t *)bit) + j);
	}

	disable_update=bzero; //0-> to update every time. 1-> don't update anymore. Once switched to 1 stays at 1
	control=bones; //used to control the disable_update properly

#include HGSAMPLER_INCLUDE

		for(k=0;k<nrotate;k++){//if sample_o is 8 bits it should rotate 8 times if sample_o is 16 bit then the loop should rotate 16 times 	
				  //At a time 8 samples will be filled up. So we need the loop to iterate 8 times to fill all the 64 samples.
			
				sample_t=bzero;
				for(j=BITS_PER_SAMPLE-1;j>=0;j--) {
					sample_t=(sample_t<<1) | (out[j]&mask);
				}
				sample_n[k]=sample_t;
				
								

				for(j=BITS_PER_SAMPLE-1;j>=0;j--) {
					out[j]=out[j]>>1;
				}

		}

		for(k=0;k<NSAMPLES;k++){
			sample[k]=(int)sample_o[k];
		}

	cursor = 0;
}
public:
	HGSAMPLER_CLASS_NAME(AES_Random & aes_rnd) :
		aes_rnd(aes_rnd)
	{
		fill();
	}
public:
	inline int operator()()
	{
		int r = sample[cursor];
		if (++cursor == NSAMPLES) fill();
		return r;
	}
public:
	double sigma() const
	{
		return Sigma;
	}
	int bitsPerSample() const
	{
		return BitsPerSample;
	}

#undef ALIGNMENT
#undef ALIGN
#undef BCONST
#undef BZERO
#undef BONES
#undef BMASK
#undef BTEST_NOT_ONES

};

#undef BITS_PER_SAMPLE
#undef HGSAMPLER_CONCAT
#undef HGSAMPLER_CONCAT2
#undef HGSAMPLER_CLASS_NAME

#undef HGSAMPLER_CLASS_SUFFIX
#undef HGSAMPLER_INCLUDE
#undef HGSAMPLER_SIGMA
#undef HGSAMPLER_N_OUT
