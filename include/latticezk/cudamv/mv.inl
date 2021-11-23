namespace LatticeZK {

template<class T>
static typename cuda_traits<T>::cuda_entry*
allocate_host_vector(cuda_memory_model memory_model, int size)
CUDA_THROWS(CUDA_EXC)
{
	cuda_traits<T>::cuda_entry* v_host = 0;
	switch (memory_model) {
	case PAGEABLE:
		v_host = (typename cuda_traits<T>::cuda_entry*)LATTICEZK_ALIGNED_ALLOC(LATTICEZK_ALIGNMENT, size);
		break;
	case PINNED:
		CUDA_ASSERT(cudaMallocHost((void**)&v_host, size), "pinned host vector allocation");
		break;
	case MAPPED:
		CUDA_ASSERT(cudaHostAlloc((void**)&v_host, size, cudaHostAllocMapped), "mapped host vector allocation");
		break;
	case UNIFIED:
		CUDA_ASSERT(cudaMallocManaged((void**)&v_host, size), "unified vector allocation");
		break;
	}
	return v_host;
}

template<class T>
static typename cuda_traits<T>::cuda_entry*
allocate_device_vector(cuda_memory_model memory_model, int size, typename cuda_traits<T>::cuda_entry* v_host)
CUDA_THROWS(CUDA_EXC)
{
	cuda_traits<T>::cuda_entry* v_device = 0;
	switch (memory_model) {
	case PAGEABLE:
	case PINNED:
		CUDA_ASSERT(cudaMalloc((void**)&v_device, size), "device vector allocation");
		break;
	case MAPPED:
		CUDA_ASSERT(cudaHostGetDevicePointer((typename cuda_traits<T>::cuda_entry**)&v_device, v_host, 0), "mapped vector allocation");
		break;
	case UNIFIED:
		v_device = v_host;
		break;
	}
	return v_device;
}

template<class T>
mv_vector<T>::mv_vector(cuda_memory_model memory_model, int chunk_length, int num_chunks, typename cuda_traits<T>::cuda_entry* v)
CUDA_THROWS(CUDA_EXC) :
	memory_model(memory_model),
	chunk_length(chunk_length),
	num_chunks(num_chunks),
	v_host_owned(v == nullptr),
	v_host(v == nullptr ? allocate_host_vector<T>(memory_model, chunk_length* num_chunks * sizeof(cuda_entry)) : v),
	v_device(allocate_device_vector<T>(memory_model, chunk_length* num_chunks * sizeof(cuda_entry), v_host))
{
}

template<class T>
mv_vector<T>::~mv_vector()
CUDA_THROWS(CUDA_EXC)
{
	switch (memory_model) {
	case PAGEABLE:
		if (v_host_owned) {
			LATTICEZK_ALIGNED_FREE(v_host);
		}
		CUDA_ASSERT(cudaFree(v_device), "device vector freeing");
		break;
	case PINNED:
		if (v_host_owned) {
			CUDA_ASSERT(cudaFreeHost(v_host), "pinned host vector allocation");
		}
		CUDA_ASSERT(cudaFree(v_device), "device vector freeing");
		break;
	case MAPPED:
		if (v_host_owned) {
			CUDA_ASSERT(cudaFree(v_host), "mapped host vector freeing");
		}
		break;
	case UNIFIED:
		if (v_host_owned) {
			CUDA_ASSERT(cudaFree(v_host), "unified vector freeing");
		}
		break;
	}
}

template<class T>
typename cuda_traits<T>::cuda_entry*
mv_vector<T>::hostData()
{
	return v_host;
}

template<class T>
const typename cuda_traits<T>::cuda_entry*
mv_vector<T>::hostData() const
{
	return v_host;
}

template<class T>
typename cuda_traits<T>::cuda_entry*
mv_vector<T>::deviceData()
{
	return v_device;
}

template<class T>
const typename cuda_traits<T>::cuda_entry*
mv_vector<T>::deviceData() const
{
	return v_device;
}

template<class T>
bool
mv_vector<T>::toDevice(cuda_stream_set& stream_set)
{
	switch (memory_model) {
	case PAGEABLE:
	case PINNED: {
		auto result = stream_set.seqdo(num_chunks, [this](cudaStream_t& stream, int start_chunk, int end_chunk) {
			CUDA_CHECK_INIT();
			int offset = chunk_length * start_chunk;
			int size = chunk_length * (end_chunk - start_chunk) * sizeof(cuda_entry);
			CUDA_CHECK(cudaMemcpyAsync(v_device + offset, v_host + offset, size, cudaMemcpyHostToDevice, stream));
			CUDA_CHECK_RETURN();
		});
		CUDA_CHECK_BOOL(result);
		break;
	}
	case MAPPED:
	case UNIFIED:
		break;
	}
	return true;
}

template<class T>
bool
mv_vector<T>::toHost(cuda_stream_set& stream_set)
{
	switch (memory_model) {
	case PAGEABLE:
	case PINNED: {
		auto result = stream_set.seqdo(num_chunks, [this](cudaStream_t& stream, int start_chunk, int end_chunk) {
			CUDA_CHECK_INIT();
			int offset = chunk_length * start_chunk;
			int size = chunk_length * (end_chunk - start_chunk) * sizeof(cuda_entry);
			CUDA_CHECK(cudaMemcpyAsync(v_host + offset, v_device + offset, size, cudaMemcpyDeviceToHost, stream));
			CUDA_CHECK_RETURN();
		});
		CUDA_CHECK_BOOL(result);
		break;
	}
	case MAPPED:
	case UNIFIED:
		break;
	}
	return true;
}

// y = Ax
// A : m-by-n matrix , x : n elementsvector , y : m elements vector
// m and n are arbitrary  positive  integers .

template<class T>
__global__ void
mv_kernel(cuda_traits<T>::cuda_entry* y, cudaTextureObject_t texObj, cudaTextureObject_t texObj2,
	const cuda_traits<T>::cuda_entry* x, int l, int m, int n);

template<class T>
mv_matrix_base<T>::mv_matrix_base(int m, int n) :
	m(m),
	n(n)
{
}

template<class T>
mv_matrix_base<T>::~mv_matrix_base()
CUDA_THROWS(CUDA_EXC)
{
}

template<class T>
bool
mv_matrix_base<T>::check_compatibility(mv_vector<T>* y_vector, const mv_vector<T>* x_vector) const
{
	return m == y_vector->chunk_length && n == x_vector->chunk_length && y_vector->num_chunks == x_vector->num_chunks;
}

template<class T>
mv_vector<T>*
mv_matrix_base<T>::new_x_vector(cuda_memory_model memory_model, int chunks, cuda_entry* v) const
{
	return new mv_vector<T>(memory_model, n, chunks, v);
}

template<class T>
mv_vector<T>*
mv_matrix_base<T>::new_y_vector(cuda_memory_model memory_model, int chunks, cuda_entry* v) const
{
	return new mv_vector<T>(memory_model, m, chunks, v);
}

template<class T>
class mv_texops_single
{
public:
	typedef typename cuda_traits<T>::cuda_entry cuda_entry;
	typedef typename cuda_traits<T>::cuda_entry4 cuda_entry4;
private:
	int m, n;
	const cuda_entry* A;
	bool ownA;
	cudaArray* d_A;
	struct cudaResourceDesc resDesc;
	struct cudaTextureDesc texDesc;
public:
	cudaTextureObject_t texObj;
private:
	cudaError_t open()
	{
		CUDA_CHECK_INIT();
		CUDA_CHECK(cudaMemcpy2DToArrayAsync(d_A, 0, 0, A, n * sizeof(cuda_entry), n * sizeof(cuda_entry), m, cudaMemcpyHostToDevice));
		CUDA_CHECK(cudaDeviceSynchronize());

		CUDA_CHECK(cudaCreateTextureObject(&texObj, &resDesc, &texDesc, NULL));
		CUDA_CHECK_RETURN();
	}
	cudaError_t close()
	{
		CUDA_CHECK_INIT();

		// Destroy texture object
		CUDA_CHECK(cudaDestroyTextureObject(texObj));

		CUDA_CHECK_RETURN();
	}
public:
	mv_texops_single(const cuda_entry* A, int m, int n, bool ownA = false) :
		m(m), n(n), A(A), ownA(ownA)
	{
		int blkNum = (m >> 4) + ((m & 15) ? 1 : 0);
		int height = blkNum << 4;
		int width = (n & 255) ? (256 * ((n >> 8) + 1)) : n;

		cudaChannelFormatDesc channelDesc = cudaCreateChannelDesc<cuda_entry4>();
		CUDA_ASSERT(cudaMallocArray(&d_A, &channelDesc, width >> 2, height), "allocating cuda array");

		// Specify texture
		memset(&resDesc, 0, sizeof(resDesc));
		resDesc.resType = cudaResourceTypeArray;
		resDesc.res.array.array = d_A;

		// Specify texture object parameters
		memset(&texDesc, 0, sizeof(texDesc));
		texDesc.addressMode[0] = cudaAddressModeBorder;
		texDesc.addressMode[1] = cudaAddressModeBorder;
		texDesc.readMode = cudaReadModeElementType;

		CUDA_ASSERT(open(), "opening");
	}
	~mv_texops_single() CUDA_THROWS(CUDA_EXC)
	{
		CUDA_ASSERT(close(), "closing");
		CUDA_ASSERT(cudaFreeArray(d_A), "freeing array");

		if (ownA) {
			delete A;
		}
	}
public:
	bool toDevice(cuda_stream_set& stream_set)
	{
		CUDA_CHECK_BOOL(close());
		CUDA_CHECK_BOOL(open());
		return true;
	}
	void callKernel(dim3 grid, dim3 threads, size_t blockShmem, cudaStream_t & stream,
		cuda_entry* y, const cuda_entry* x, int l, int m, int n) const
	{
		mv_kernel<T> <<<grid, threads, blockShmem, stream >>> (y, texObj, texObj, x, l, m, n);
		CUDA_ASSERT(cudaGetLastError(), "invoke kernel single");
	}
};

template<class T, class H>
class mv_texops_double
{
public:
	typedef typename cuda_traits<T>::cuda_entry cuda_entry;
	typedef typename cuda_traits<T>::cuda_entry4 cuda_entry4;
	typedef typename cuda_traits<H>::cuda_entry cuda_hentry;
	typedef typename cuda_traits<H>::cuda_entry4 cuda_hentry4;
private:
	static cuda_hentry* halfOf(const cuda_entry* A, int m, int n, bool lowHalf)
	{
		constexpr size_t shift = sizeof(cuda_hentry) << 3;
		size_t lshift = lowHalf ? shift : 0;
		cuda_hentry* halfA = new cuda_hentry[m * n * sizeof(cuda_hentry)];
		for (int i = 0; i < m * n; i++) {
			halfA[i] = (cuda_hentry)(((A[i]) << lshift) >> shift);
		}
		return halfA;
	}
private:
	mv_texops_single<H> texops1, texops2;
public:
	mv_texops_double(const cuda_entry* A, int m, int n) :
		texops1(halfOf(A, m, n, true), m, n), texops2(halfOf(A, m, n, false), m, n)
	{
	}
	~mv_texops_double() CUDA_THROWS(CUDA_EXC)
	{
	}
public:
	bool toDevice(cuda_stream_set& stream_set)
	{
		return texops1.toDevice(stream_set) && texops2.toDevice(stream_set);
	}
	void callKernel(dim3 grid, dim3 threads, size_t blockShmem, cudaStream_t& stream,
		cuda_entry* y, const cuda_entry* x, int l, int m, int n) const
	{
		mv_kernel<T> <<< grid, threads, blockShmem, stream >>> (y, texops1.texObj, texops2.texObj, x, l, m, n);
		CUDA_ASSERT(cudaGetLastError(), "invoke kernel double");
	}
};

template<class T>
class mv_texops : public mv_texops_single<T>
{
public:
	mv_texops(const cuda_entry* A, int m, int n) :
		mv_texops_single<T>(A, m, n)
	{
	}
	~mv_texops() CUDA_THROWS(CUDA_EXC)
	{
	}
};

template<>
class mv_texops<long long> : public mv_texops_double<long long, long>
{
public:
	mv_texops(const cuda_entry* A, int m, int n) :
		mv_texops_double<long long, long>(A, m, n)
	{
	}
	~mv_texops() CUDA_THROWS(CUDA_EXC)
	{
	}
};

template<class T>
mv_matrix_tex<T>::mv_matrix_tex(const mv_matrix_tex<T>::cuda_entry* A, int m, int n) :
	mv_matrix_base<T>(m, n), A(A), texops(A, m, n)
{
}

template<class T>
mv_matrix_tex<T>::~mv_matrix_tex()
CUDA_THROWS(CUDA_EXC)
{
}

template<class T>
bool
mv_matrix_tex<T>::toDevice(cuda_stream_set& stream_set)
{
	return texops.toDevice(stream_set);
}

template<class T>
bool
mv_matrix_tex<T>::multiply(cuda_stream_set& stream_set, mv_vector<T>* y_vector, const mv_vector<T>* x_vector) const
{
	if (!check_compatibility(y_vector, x_vector)) {
		return false;
	}
	int L = y_vector->num_chunks;
	int blkNum = (m >> 4) + ((m & 15) ? 1 : 0);
	int chkNum = (L >> 2) + ((L & 3) ? 1 : 0);
	dim3 threads(16, 16), grid(blkNum, chkNum);

	auto d_y = y_vector->v_device;
	auto d_x = x_vector->v_device;

	auto result = stream_set.seqdo(L, [this, &grid, &threads, &d_y, &d_x](cudaStream_t& stream, int start_chunk, int end_chunk) {
		CUDA_CHECK_INIT();
		texops.callKernel(grid, threads, 0, stream, d_y + start_chunk * m, d_x + start_chunk * n, end_chunk - start_chunk, m, n);
		CUDA_CHECK_RETURN();
	});
	CUDA_CHECK_BOOL(result);
	return true;
}

template<class T4>
__device__ T4
doubletex2D(cudaTextureObject_t texObj, cudaTextureObject_t texObj2, float x, float y)
{
	return tex2D<T4>(texObj, x, y);
}

template<>
__device__ longlong4
doubletex2D<longlong4>(cudaTextureObject_t texObj, cudaTextureObject_t texObj2, float x, float y)
{
	constexpr size_t shift = sizeof(long) << 3;
	long4 low = tex2D<long4>(texObj, x, y);
	long4 high = tex2D<long4>(texObj2, x, y);
	longlong4 r;
	r.x = ((long long)low.x) | (((long long)high.x) << shift);
	r.y = ((long long)low.y) | (((long long)high.y) << shift);
	r.z = ((long long)low.z) | (((long long)high.z) << shift);
	r.w = ((long long)low.w) | (((long long)high.w) << shift);
	return r;
}

template<class T>
__device__ void
mv_kernel(cuda_traits<T>::cuda_entry* y, cudaTextureObject_t texObj, cudaTextureObject_t texObj2,
	const cuda_traits<T>::cuda_entry* x, int m, int n)
{
	typedef typename cuda_traits<T>::cuda_entry cuda_entry;
	typedef typename cuda_traits<T>::cuda_entry4 cuda_entry4;
	const int bx = blockIdx.x, tx = threadIdx.x, ty = threadIdx.y;
	__shared__ cuda_entry xs[16][16];
	__shared__ cuda_entry Ps[16][16];
	cuda_entry4 a;
	cuda_entry* Psptr = (cuda_entry*)Ps + (ty << 4) + tx;
	int ay = (bx << 4) + ty;
	const cuda_entry* xptr = x + (ty << 4) + tx;
	cuda_entry* xsptr = (cuda_entry*)xs + (tx << 2);

	*Psptr = 0;
	int i;
	for (i = 0; i < (n & ~255); i += 256, xptr += 256) {
		xs[ty][tx] = *xptr;
		__syncthreads();
		int ax = tx + (i >> 2);
		a = doubletex2D<cuda_entry4>(texObj, texObj2, ax, ay);
		*Psptr += a.x * *(xsptr)+a.y * *(xsptr + 1) + a.z * *(xsptr + 2) + a.w * *(xsptr + 3);
		a = doubletex2D<cuda_entry4>(texObj, texObj2, ax + 16, ay);
		*Psptr += a.x * *(xsptr + 64) + a.y * *(xsptr + 65) + a.z * *(xsptr + 66) + a.w * *(xsptr + 67);
		a = doubletex2D<cuda_entry4>(texObj, texObj2, ax + 32, ay);
		*Psptr += a.x * *(xsptr + 128) + a.y * *(xsptr + 129) + a.z * *(xsptr + 130) + a.w * *(xsptr + 131);
		a = doubletex2D<cuda_entry4>(texObj, texObj2, ax + 48, ay);
		*Psptr += a.x * *(xsptr + 192) + a.y * *(xsptr + 193) + a.z * *(xsptr + 194) + a.w * *(xsptr + 195);
	}

	if (i + (ty << 4) + tx < n) {
		xs[ty][tx] = *xptr;
	}
	int j;
	for (j = 0; j < ((n - i) >> 6); j++, xsptr += 64) {
		a = doubletex2D<cuda_entry4>(texObj, texObj2, tx + (i >> 2) + (j << 4), ay);
		*Psptr += a.x * xsptr[0] + a.y * xsptr[1] + a.z * xsptr[2] + a.w * xsptr[3];
	}
	int remain = (n - i) & 63;
	if ((tx << 2) < remain) {
		a = doubletex2D<cuda_entry4>(texObj, texObj2, tx + (i >> 2) + (j << 4), ay);
		*Psptr += a.x * *xsptr++;
	}
	if ((tx << 2) + 1 < remain) *Psptr += a.y * *xsptr++;
	if ((tx << 2) + 2 < remain) *Psptr += a.z * *xsptr++;
	if ((tx << 2) + 3 < remain) *Psptr += a.w * *xsptr;

#pragma unroll
	for (i = 8; i > 0; i >>= 1) {
		if (tx < i) *Psptr += *(Psptr + i);
		__syncthreads();
	}

	if (ty == 0 && (bx << 4) + tx < m) y[(bx << 4) + tx] = Ps[tx][0];
	__syncthreads();
}

template<class T>
__global__ void
mv_kernel(cuda_traits<T>::cuda_entry* y0, cudaTextureObject_t texObj, cudaTextureObject_t texObj2,
	const cuda_traits<T>::cuda_entry* x0, int l, int m, int n)
{
	typedef typename cuda_traits<T>::cuda_entry cuda_entry;
	for (int k = blockIdx.y; k < l; k += gridDim.y) {
		cuda_entry* y = y0 + k * m;
		const cuda_entry* x = x0 + k * n;
		mv_kernel<T>(y, texObj, texObj2, x, m, n);
	}
}

} // namespace LatticeZK
