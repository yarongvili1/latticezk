// Basic Gaussian sampler tool

#include <stdio.h>
#include "latticezk/util/cpucycles.hpp"
#include "latticezk/gaussian/gsampler.hpp"

using namespace LatticeZK;

typedef HalfGaussianSampler_S215_N10 HalfGaussianSampler; // alternatively, HalfGaussianSampler_S2_N5

template<typename Sampler>
void sample(Sampler &sampler)
{
	static constexpr int NHIST = (1 << (1 + HalfGaussianSampler::BitsPerSample));
	unsigned long long int hist[NHIST];
	static constexpr int NSAMPLES = HalfGaussianSampler::NSAMPLES;

	long int i;
	int sample;

	int repeat=100000;
	volatile unsigned long long int clock1, clock2, clock3;
	clock3 = 0;
	clock1 = cpucycles();
	for(i=0;i<repeat*NSAMPLES;i++){	
		sample = sampler();
		hist[sample + (NHIST / 2)]++;
	}
	clock2 = cpucycles();
	clock3 += clock2 - clock1;
	printf("Number of rejections: %lu\n",sampler.get_rejections());
	printf("Time for all samples: %lld\n",clock3);
	printf("Number of samples: %d\n",repeat*NSAMPLES);
	printf("Time per sample: %lld\n",clock3/repeat/NSAMPLES);
	for(i=0;i<NHIST;i++) {
		printf(" %ld : %llu",i - (NHIST / 2),hist[i]);
	}
	printf("\n");
}

void sample0(AES_Random & aes_rnd)
{
	PlainGaussianSampler<HalfGaussianSampler> sampler(aes_rnd);
	sample(sampler);
}

void sample1(AES_Random & aes_rnd, double mu, double sigma)
{
	AdjustedGaussianSampler<HalfGaussianSampler> sampler(aes_rnd, mu, sigma);
	sample(sampler);
}

int main(int argc, char **argv){
	AES_Random aes_rnd;
	aes_rnd.reseed(1);
	if (argc < 3) {
		sample0(aes_rnd);
		return 0;
	}
	double mu = atof(argv[1]);
	double sigma = atof(argv[2]);
	sample1(aes_rnd, mu, sigma);
	return 1;
}
