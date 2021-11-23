#ifndef __LATTICEZK_CUDAMV_MV_HPP_
#define __LATTICEZK_CUDAMV_MV_HPP_

#include "latticezk/cudamv/common.hpp"

namespace LatticeZK {

template<class T>
class mv_vector
{
public:
	typedef typename cuda_traits<T>::cuda_entry cuda_entry;
public:
	const cuda_memory_model memory_model;
	const int chunk_length;
	const int num_chunks;
	const bool v_host_owned;
	cuda_entry* v_host;
	cuda_entry* v_device;
public:
	mv_vector(cuda_memory_model memory_model, int chunk_length, int num_chunks, cuda_entry* v_host = nullptr) CUDA_THROWS(CUDA_EXC);
	virtual ~mv_vector() CUDA_THROWS(CUDA_EXC);

	cuda_entry* hostData();
	const cuda_entry* hostData() const;
	cuda_entry* deviceData();
	const cuda_entry* deviceData() const;

	bool toDevice(cuda_stream_set& stream_set);
	bool toHost(cuda_stream_set& stream_set);
};

template<class T>
class mv_matrix_base {
public:
	typedef typename cuda_traits<T>::cuda_entry cuda_entry;
	typedef typename cuda_traits<T>::cuda_entry4 cuda_entry4;
public:
	const int m, n;
protected:
	bool check_compatibility(mv_vector<T>* y_vector, const mv_vector<T>* x_vector) const;
public:
	mv_matrix_base(int m, int n);
	virtual ~mv_matrix_base() CUDA_THROWS(CUDA_EXC);

	mv_vector<T>* new_x_vector(cuda_memory_model memory_model, int chunks, cuda_entry* v = nullptr) const;
	mv_vector<T>* new_y_vector(cuda_memory_model memory_model, int chunks, cuda_entry* v = nullptr) const;

	virtual bool multiply(cuda_stream_set& stream_set, mv_vector<T>* y_vector, const mv_vector<T>* x_vector) const = 0;
};

template<class T> class mv_texops;

template<class T>
class mv_matrix_tex : public mv_matrix_base<T> {
private:
	const cuda_entry* A;
	mv_texops<T> texops;
public:
	mv_matrix_tex(const cuda_entry* A, int m, int n);
	~mv_matrix_tex() CUDA_THROWS(CUDA_EXC);

	bool toDevice(cuda_stream_set & stream_set);
	virtual bool multiply(cuda_stream_set & stream_set, mv_vector<T>* y_vector, const mv_vector<T>* x_vector) const;
};

} // namespace LatticeZK

#include "latticezk/cudamv/mv.inl"

#endif // __LATTICEZK_CUDAMV_MV_HPP_
