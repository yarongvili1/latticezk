#include <iostream>
#include "latticezk/gaussian/facct.hpp"
#include "latticezk/util/cpucycles.hpp"

namespace LatticeZK {
	namespace Main {
		// main using CPU
		extern int main();
	}
	namespace Cuda {
		// main using GPU
		extern int main();
		// Facct-sampling using GPU
		extern void facct_sample_215();
		extern void facct_sample_1000000();
		extern void facct_sample_2000000000();
	}
}

// Facct-sampling using CPU
template<uint32_t Sigma>
void facct_sample()
{
	typedef LatticeZK::FacctGaussianSampler<Sigma> Facct;
	unsigned char seed[32] = { '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1' };
	LatticeZK::AES_Random aes_rnd;
	aes_rnd.reseed(seed);
	LatticeZK::BytesSampler byte_sampler(aes_rnd);
	int nsamples = 1 << 20;
	int64_t sum = 0;
	Facct facct(byte_sampler);
	uint64_t t0 = cpucycles();
	for (int i = 0; i < nsamples; i++) {
		sum += facct();
	}
	uint64_t t1 = cpucycles();
	std::cerr << nsamples << " samples took " << (t1 - t0) << " cycles (sum=" << sum << ")." << std::endl;
	for (int i = 0; i < 1000; i++) {
		std::cerr << facct() << " ";
	}
	std::cerr << std::endl;
}

void facct_sample_215()
{
	facct_sample<215>();
}

void facct_sample_1000000()
{
	facct_sample<1000000>();
}

void facct_sample_2000000000()
{
	facct_sample<2000000000>();
}

int main()
{
	int r;
	std::cerr << "=== Running on Main system ===" << std::endl;
	r = LatticeZK::Main::main();
	if (r != 0) return r;
	std::cerr << "=== Running on Cuda system ===" << std::endl;
	r = LatticeZK::Cuda::main();
	if (r != 0) return r;

	std::cerr << "=== Running Facct sigma=215 on Main system ===" << std::endl;
	facct_sample_215();
	std::cerr << "=== Running Facct sigma=215 on Cuda system ===" << std::endl;
	LatticeZK::Cuda::facct_sample_215();

	std::cerr << "=== Running Facct sigma=1M on Main system ===" << std::endl;
	facct_sample_1000000();
	std::cerr << "=== Running Facct sigma=1M on Cuda system ===" << std::endl;
	LatticeZK::Cuda::facct_sample_1000000();

	std::cerr << "=== Running Facct sigma=2G on Main system ===" << std::endl;
	facct_sample_2000000000();
	std::cerr << "=== Running Facct sigma=2G on Cuda system ===" << std::endl;
	LatticeZK::Cuda::facct_sample_2000000000();

	return 0;
}