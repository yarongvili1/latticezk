#ifndef __LATTICEZK_CUDAMV_CUDAMATRIX_HPP_
#define __LATTICEZK_CUDAMV_CUDAMATRIX_HPP_

// Extending matrices to the GPU
// An extended matrix is placed in CPU and GPU memory
// Syncing to the GPU is needed after the CPU copy is modified

#include "latticezk/matrix.hpp"
#include "latticezk/matrixops.hpp"
#include "latticezk/cudamv/mv.hpp"

namespace LatticeZK {

// A column-major-order matrix extended to a CUDA vector
template<typename T>
class CudaVector : public Matrix<T, ColumnMajorOrder>
{
private:
	mv_vector<T> mv_vec;
private:
	CudaVector(const CudaVector & other) = delete;
	CudaVector(const CudaVector && other) = delete;
public:
	CudaVector(matdim_t n_rows, matdim_t n_cols, cuda_memory_model memory_model = DEFAULT_CUDA_MEMORY_MODEL) :
		Matrix<T, ColumnMajorOrder>(n_rows, n_cols), mv_vec(memory_model, n_rows, n_cols, Data())
	{
	}
public:
	mv_vector<T> & GetMvVector()
	{
		return mv_vec;
	}
	const mv_vector<T> & GetMvVector() const
	{
		return mv_vec;
	}
};

// A row-major-order matrix extended to a CUDA matrix
template<typename T>
class CudaMatrix : public Matrix<T, RowMajorOrder>
{
private:
	mv_matrix_tex<T> mv_mat;
private:
	CudaMatrix(const CudaMatrix & other) = delete;
	CudaMatrix(const CudaMatrix && other) = delete;
public:
	CudaMatrix(matdim_t n_rows, matdim_t n_cols) :
		Matrix<T, RowMajorOrder>(n_rows, n_cols), mv_mat(Data(), n_rows, n_cols)
	{
	}
public:
	mv_matrix_tex<T> & GetMvMatrix()
	{
		return mv_mat;
	}
	const mv_matrix_tex<T> & GetMvMatrix() const
	{
		return mv_mat;
	}
};

// CUDA matrix operations
// An instance of this class is used (in place of MatrixOps) with extended matrices
template<typename T>
class CudaMvOps
{
public:
	typedef T data_t;
	typedef CudaMatrix<T> RowMajorMatrix;
	typedef CudaVector<T> ColumnMajorMatrix;
private:
	cuda_stream_set & stream_set;
	MatrixOps<T> matops;
public:
	CudaMvOps(cuda_stream_set & stream_set) :
		stream_set(stream_set)
	{
	}
public:
	bool Copy(CudaMatrix<T>& dst, const CudaMatrix<T>& src)
	{
		return matops.Copy(dst, src) && Sync(dst);
	}
	bool Copy(CudaVector<T>& dst, const CudaVector<T>& src)
	{
		return matops.Copy(dst, src) && Sync(dst);
	}
	bool Sync(RowMajorMatrix & mat)
	{
		return mat.GetMvMatrix().toDevice(stream_set);
	}
	bool Sync(ColumnMajorMatrix & mat)
	{
		return mat.GetMvVector().toDevice(stream_set);
	}
	bool Multiply(const RowMajorMatrix &a, const ColumnMajorMatrix &b, ColumnMajorMatrix &c)
	{
		return a.GetMvMatrix().multiply(stream_set, &c.GetMvVector(), &b.GetMvVector()) && c.GetMvVector().toHost(stream_set);
	}
};

} // namespace LatticeZK

#endif // __LATTICEZK_CUDAMV_CUDAMATRIX_HPP_
