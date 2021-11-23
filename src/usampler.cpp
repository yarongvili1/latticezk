#include <stdio.h>
#include "latticezk/util/cpucycles.hpp"
#include "latticezk/uniform/usampler.hpp"

using namespace LatticeZK;

void sample_u64(AES_Random & aes_rnd)
{
	long int i;
	uint64_t sample, sum = 0;
	U64Sampler sampler(aes_rnd);

	int repeat=1<<24;
	volatile unsigned long long int clock1, clock2, clock3;
	clock3 = 0;
	clock1 = cpucycles();
	for(i=0;i<repeat;i++){	
		sample = sampler();
		sum += sample;
	}
	clock2 = cpucycles();
	clock3 += clock2 - clock1;
	printf("Cycles for all samples: %lld\n",clock3);
	printf("Number of samples: %d\n",repeat);
	printf("Cycles per sample: %f\n",1.0*clock3/repeat);
	printf("Sum: %lu\n",sum);
}

int main(){
	AES_Random aes_rnd;
	aes_rnd.reseed(1);
	sample_u64(aes_rnd);
	return 0;
}
