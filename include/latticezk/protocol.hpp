#ifndef __LATTICEZK_PROTOCOL_HPP
#define __LATTICEZK_PROTOCOL_HPP

#include "latticezk/prover.hpp"
#include "latticezk/gaussian//facct.hpp"

namespace LatticeZK {

// Implementation of Lattice-based NIZK protocol:
// 	   "Sub-Linear Lattice-Based Zero-Knowledge	Arguments for Arithmetic Circuits", Baum et al
//     https://eprint.iacr.org/2018/560.pdf
// Supports running matrix-multiplications in the GPU
//
// Parameters:
//   matops	        defines how matrix operations are carried out
//   s_bits	        number of bits the secret is limited to
//   lambda	        security parameter
//   n,rho,r,v,l    protocol parameters from the paper     
template<typename MatOps, uint64_t sigma>
void run_protocol(MatOps & matops, int s_bits, uint32_t lambda, matdim_t n, double rho, matdim_t r, matdim_t v, matdim_t l, bool debug=false)
{
	typedef typename MatOps::data_t data_t;
	typedef typename MatOps::RowMajorMatrix RowMajorMatrix;
	typedef typename MatOps::ColumnMajorMatrix ColumnMajorMatrix;

	double s = (double)l * (1LL << (s_bits - 1));

	AES_Random aes_rnd;
	aes_rnd.reseed();
	BitsSampler bsampler(aes_rnd, s_bits);
	MatrixSampler<UIntSampler<data_t>> asampler(aes_rnd);
	MatrixSampler<BitsSampler> ssampler(bsampler);
	RowMajorMatrix matA(r,v);
	ColumnMajorMatrix matS(v,l);
	LATTICEZK_TIMER_START("sampling A");
	asampler(matA);
	LATTICEZK_TIMER_END;
	LATTICEZK_TIMER_START("sampling S");
	ssampler(matS);
	LATTICEZK_TIMER_END;
#ifndef __CUDACC__
	if (debug) LATTICEZK_LOG(matA << std::endl << std::endl << matS << std::endl);
#endif
	auto prover = Prover<data_t, FacctGaussianSampler<sigma>, MatOps>::Create(matops, matA, matS, lambda, s, n, rho);
	if (prover == nullptr) {
		std::cerr << "No prover" << std::endl;
		return;
	}
	Proof<data_t, MatOps> proof(r, v, l, n, prover->GetB());
	uint64_t draws = 0;
	LATTICEZK_TIMER_START("proving");
	draws = prover->Prove(aes_rnd, proof);
	LATTICEZK_TIMER_END;
	delete prover;

	bool verified = false;
	Verifier<data_t, MatOps> verifier(matops, proof.r, proof.v, proof.l, proof.n, proof.B);
	LATTICEZK_TIMER_START("verifying");
	verified = verifier.Verify(proof);
	LATTICEZK_TIMER_END;
	LATTICEZK_LOG("draws=" << draws << " verified=" << verified);
}

} // namespace LatticeZK

#endif // __LATTICEZK_PROTOCOL_HPP
