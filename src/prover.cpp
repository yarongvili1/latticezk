#include <iostream>
#include "latticezk/common.hpp"
#include "latticezk/util/aes_rnd.hpp"
#include "latticezk/util/cpucycles.hpp"
#include "latticezk/prover.hpp"
#ifdef __CUDACC__
	#include "latticezk/cudamv/cudamatrix.hpp"
#else
	#include "latticezk/matrixops.hpp"
	#include "latticezk/matrixext.hpp"
#endif
#include "latticezk/protocol.hpp"

using namespace LatticeZK;

namespace LatticeZK {
#ifdef __CUDACC__
	namespace Cuda {
#else
	namespace Main {
#endif

// runs the protocol on the CPU or the GPU, depending on the compiler used during build
template<typename data_t, uint64_t sigma>
void run_protocol(int s_bits, uint32_t lambda, matdim_t n, double rho, matdim_t r, matdim_t v, matdim_t l, bool debug=false)
{
#ifdef __CUDACC__
	cuda_stream_set stream_set(16);
	CudaMvOps<data_t> matops(stream_set);
	run_protocol<CudaMvOps<data_t>, sigma>(matops, s_bits, lambda, n, rho, r, v, l, debug);
#else
	MatrixOps<data_t> matops;
	run_protocol<MatrixOps<data_t>, sigma>(matops, s_bits, lambda, n, rho, r, v, l, debug);
#endif
}

// runs the protocol with default parameters
template<typename data_t>
void run_protocol_default()
{
	constexpr uint32_t sigma = 2000000000;
	int s_bits = 7;
	uint32_t lambda = 80;
	matdim_t n = 100;
	double rho = 2;
	matdim_t r = 100, v = 3000, l = 3000;
	bool debug = false;

	run_protocol<data_t, sigma>(s_bits, lambda, n, rho, r, v, l, debug);
}

#ifdef __CUDACC__
	} // namespace Cuda
#else
	} // namespace Main
#endif
} // namespace LatticeZK

#ifdef _WIN32
namespace LatticeZK {
#ifdef __CUDACC__
	namespace Cuda {
#else
	namespace Main {
#endif
#endif

int main()
{
#ifdef __CUDACC__
	using namespace LatticeZK::Cuda;
#else
	using namespace LatticeZK::Main;
#endif
	run_protocol_default<int64_t>();
#ifdef __CUDACC__
	CUDA_ASSERT(cudaDeviceReset(), "resetting device");
#endif
	return 0;
}

#ifdef _WIN32
#ifdef __CUDACC__
	} // namespace Cuda
#else
	} // namespace Main
#endif
} // namespace LatticeZK
#endif