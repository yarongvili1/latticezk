#ifndef __LATTICEZK_UNIFORM_USAMPLER_HPP_
#define __LATTICEZK_UNIFORM_USAMPLER_HPP_

// Uniform samplers

#include <algorithm>
#include <cstring>
#include "latticezk/util/aes_rnd.hpp"
#include "latticezk/common.hpp"

namespace LatticeZK
{

// Uniformly samples 128-bits at-a-time using AES in CTR-mode
class AESSampler
{
protected:
	uint64_t samples[2];
	int cursor;
	AES_Random & aes_rnd;
protected:
	inline void fill()
	{
		aes_rnd.random_bytes((uint8_t *)&samples);
		cursor = 0;
	}
public:
	AESSampler(AES_Random & aes_rnd) :
		aes_rnd(aes_rnd)
	{
		fill();
	}
};

template<typename T>
constexpr int width_bits_of()
{
	return sizeof(T) == 1 ? 3 : sizeof(T) == 2 ? 4 : sizeof(T) == 4 ? 5 : sizeof(T) == 8 ? 6 : -1;
}

// Uniformly samples an int-type at-a-time using AES in CTR-mode
template<typename T, int width_bits = width_bits_of<T>()>
class UIntSampler :
       public AESSampler
{
	static_assert(width_bits >= 0 && width_bits <= 6, "invalid width_bits");
private:
	static constexpr int cursor_bits = 1 + (6 - width_bits);
	static constexpr int cursor_low_bits = cursor_bits - 1;
	static constexpr int cursor_low_mask = (1 << cursor_low_bits) - 1;
LATTICEZK_BEGIN_UNSIGNED_UNARY_MINUS
	static constexpr uint64_t cursor_pos_mask = (-1ULL) >> (64 - (1 << width_bits));
LATTICEZK_END_UNSIGNED_UNARY_MINUS
	static constexpr int cursor_limit = 1 << cursor_bits;
public:
	UIntSampler(AES_Random & aes_rnd) :
		AESSampler(aes_rnd)
	{
	}
	inline T operator()()
	{
		int cursor_pos_bits = (cursor & cursor_low_mask) << width_bits;
		T r = (T)((samples[cursor >> cursor_low_bits] >> cursor_pos_bits) & cursor_pos_mask);
		if (++cursor == cursor_limit) fill();
		return r;
	}
};

typedef UIntSampler<int32_t> U32Sampler;
typedef UIntSampler<int64_t> U64Sampler;
typedef UIntSampler<int, 0> BitSampler;

// Uniformly samples k-bits at-a-time using AES in CTR-mode
class BitsSampler :
       public AESSampler
{
private:
	int n_bits;
public:
	BitsSampler(AES_Random & aes_rnd, int n_bits) :
		AESSampler(aes_rnd), n_bits(n_bits)
	{
	}
	inline int64_t operator()()
	{
		const int c_mask = (1 << 6) - 1;
		int r_bits = n_bits;
		int64_t r = 0;
		do {
			int n = std::min(64 - (cursor & c_mask), r_bits);
			int64_t n_mask = (1 << n) - 1;
			r |= ((samples[cursor >> 6] >> (cursor & c_mask)) & n_mask);
			r_bits -= n;
			cursor += n;
			if (cursor == 128) fill();
		} while (r_bits > 0);
		return (r << (64 - n_bits)) >> (64 - n_bits);
	}
};

// Uniformly samples k-bytes at-a-time using SES in CTR-mode
class BytesSampler
{
private:
	AES_Random & aes_rnd;
public:
	BytesSampler(AES_Random & aes_rnd) :
		aes_rnd(aes_rnd)
	{
	}
	inline unsigned char * operator()(unsigned char * r, unsigned long long rlen)
	{
		unsigned char ct[16];
		unsigned long long num_of_blocks = rlen >> 4;
		unsigned long long i;
		
		for (i = 0; i < num_of_blocks; i++)
		{
			aes_rnd.random_bytes(r + (i << 4));
		}
		
		if (rlen & 0x0f)
		{
			aes_rnd.random_bytes(ct);
			 
			std::memcpy(r + (i << 4), ct, rlen & 0x0f);
		}
		return r;
	}
};

} // namespace LatticeZK


#endif // __LATTICEZK_UNIFORM_USAMPLER_HPP_
