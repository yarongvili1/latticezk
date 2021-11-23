#ifndef __LATTICEZK_CUDAMV_COMMON_HPP_
#define __LATTICEZK_CUDAMV_COMMON_HPP_

// Common CUDA facilities
//   - error-handling
//   - mapping basic arithmetic types (like int and double) to the corresponding 4-tuple CUDA ones (like int4 and double4)
//   - cuda_memory_model definitions
//   - cuda_stream_set operations

#include <exception>
#include <vector>
#include "cublas_v2.h"
#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#define CUDA_EXC std::runtime_error
#define CUDA_THROWS(...) noexcept(false)

#define CUDA_CHECK_BOOL(expr) if (cudaSuccess != (expr)) return false;
#define CUDA_CHECK_INIT() cudaError_t cudaStatus = cudaSuccess;
#define CUDA_CHECK(expr) if (cudaSuccess != (cudaStatus = (expr))) return cudaStatus;
#define CUDA_CHECK_RETURN() return cudaStatus;
#define CUDA_ASSERT(expr, msg) if ((expr) != cudaSuccess) throw CUDA_EXC(msg);
#define CUDA_ALERT(expr, msg) \
	do { \
		cudaError_t cudaStatus = (expr); \
		if (cudaStatus != cudaSuccess) {\
			std::cerr << (std::string(cudaGetErrorString(cudaStatus)) + " : " + msg) << std::endl; \
		} \
	} while(0);

namespace LatticeZK {

template<class T>
class cuda_traits;

#define CUDA_TRAITS(type, fmt) \
template<> \
class cuda_traits<type> \
{ \
public: \
	typedef type cuda_entry; \
	typedef type##4 cuda_entry4; \
};

#define CUDA_TRAITS2(type, type4, fmt) \
template<> \
class cuda_traits<type> \
{ \
public: \
	typedef type cuda_entry; \
	typedef type4 cuda_entry4; \
};

CUDA_TRAITS(short,  "%hi")
CUDA_TRAITS(int,    "%d")
CUDA_TRAITS(long,   "%ld")
CUDA_TRAITS2(long long, longlong4, "%lld")
CUDA_TRAITS(float,  "%f")
CUDA_TRAITS(double, "%lf")

// see https://medium.com/analytics-vidhya/cuda-memory-model-823f02cef0bf
enum cuda_memory_model
{
	PAGEABLE,
	PINNED,
	MAPPED,
	UNIFIED
};

constexpr cuda_memory_model DEFAULT_CUDA_MEMORY_MODEL = PINNED;

// Support for running multiple concurrent streams of invocations on the GPU
class cuda_stream_set
{
private:
	int count;
	std::vector<cudaStream_t> streams;
public:
	cuda_stream_set(int count) CUDA_THROWS(CUDA_EXC) :
		count(count),
		streams(count)
	{
		for (int i = 0; i < count; i++) {
			CUDA_ASSERT(cudaStreamCreate(&streams[i]), "creating stream");
		}
	}
	~cuda_stream_set() CUDA_THROWS(CUDA_EXC)
	{
		CUDA_ASSERT(cudaDeviceSynchronize(), "device synchronize");
		for (int i = 0; i < count; i++) {
			CUDA_ASSERT(cudaStreamDestroy(streams[i]), "destroying stream");
		}
	}

	int size() const
	{
		return count;
	}
	cudaStream_t& operator[](int index)
	{
		return streams[index];
	}

	cudaError_t sync(int index)
	{
		CUDA_CHECK_INIT();
		CUDA_CHECK(cudaStreamSynchronize(streams[index]));
		CUDA_CHECK_RETURN();
	}
	cudaError_t sync()
	{
		CUDA_CHECK_INIT();
		for (int i = 0; i < count; i++) {
			sync(i);
		}
		CUDA_CHECK_RETURN();
	}

	template<class L>
	cudaError_t
	seqdo(int total_chunks, L cudaFunc, bool sync = true)
	{
		CUDA_CHECK_INIT();
		int set_size = std::min(size(), total_chunks);
		int start_chunk = 0;
		for (int i = 0; i < set_size; i++) {
			int end_chunk = (total_chunks * (i + 1)) / set_size;
			CUDA_CHECK(cudaFunc(streams[i], start_chunk, end_chunk));
			start_chunk = end_chunk;
		}
		if (sync) {
			CUDA_CHECK(cudaDeviceSynchronize());
		}
		CUDA_CHECK_RETURN();
	}
};

} // namespace LatticeZK

#endif // __LATTICEZK_CUDAMV_COMMON_HPP_
