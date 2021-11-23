#ifndef __LATTICEZK_MATRIX_HPP_
#define __LATTICEZK_MATRIX_HPP_

// Matrix classes supporting
//   - integer-type modular arithmetic operations (e.g., mod 2^16 or mod 2^32)
//   - parallelization via OpenMP
//   - RMO (row-major-order) and CMO (column-major-order)
//   - matrix multiplication in the form (RMO,CMO) -> CMO
//   - Frobenius inner-product and norm
//
// The set of operations is designed to support the lattice-based NIZK protocol
//
// BLAS was considered but rejected for not supporting integer-type modular arithmetic operations

#include <malloc.h>
#include <string.h>
#include <cmath>
#include "latticezk/common.hpp"
#include "latticezk/log.hpp"

#define LATTICEZK_MATDOT_INCREMENT (1 << 10)
#define LATTICEZK_MATDOT_THRESHOLD (1 << 14)
#define LATTICEZK_MATMUL_THRESHOLD1 (1 << 5)
#define LATTICEZK_MATMUL_THRESHOLD2 (1 << 10)

namespace LatticeZK {

typedef int32_t matdim_t;

class RowMajorOrder
{
private:
	const matdim_t n_rows, n_cols;
public:
	RowMajorOrder(matdim_t n_rows, matdim_t n_cols) :
		n_rows(n_rows), n_cols(n_cols)
	{
	}
public:
	inline matdim_t operator()(matdim_t i) const
	{
		return i;
	}
	inline matdim_t operator()(matdim_t i, matdim_t j) const
	{
		return i * n_cols + j;
	}
};

class ColumnMajorOrder
{
private:
	const matdim_t n_rows, n_cols;
public:
	ColumnMajorOrder(matdim_t n_rows, matdim_t n_cols) :
		n_rows(n_rows), n_cols(n_cols)
	{
	}
public:
	inline matdim_t operator()(matdim_t i) const
	{
		matdim_t j = i / n_rows;
		return this->operator()(i - j, j);
	}
	inline matdim_t operator()(matdim_t i, matdim_t j) const
	{
		return j * n_rows + i;
	}
};

template <typename T, typename Order>
class Matrix;

template<typename T>
bool MatrixToRowMajorOrder(const Matrix<T, ColumnMajorOrder> & a, Matrix<T, RowMajorOrder> & t)
{
	if (a.NumRows() != t.NumRows() || a.NumCols() != t.NumCols()) {
		return false;
	}
	matdim_t i, j, iend = a.NumRows(), jend = a.NumCols();
#if defined(_OPENMP)
	#pragma omp parallel shared(a, t) private(i, j) if (iend*jend > LATTICEZK_MATDOT_THRESHOLD)
#endif
  	{
#if defined(_OPENMP)
		#pragma omp for schedule(static)
#endif
		for (j = 0; j < jend; j++) {
			for (i = 0; i < iend; i++) {
				t(i, j) = a(i, j);
			}
		}
	}
	return true;
}

template<typename T>
bool MatrixToColumnMajorOrder(const Matrix<T, RowMajorOrder> & a, Matrix<T, ColumnMajorOrder> & t)
{
	if (a.NumRows() != t.NumRows() || a.NumCols() != t.NumCols()) {
		return false;
	}
	matdim_t i, j, iend = a.NumRows(), jend = a.NumCols();
#if defined(_OPENMP)
	#pragma omp parallel shared(a, t) private(i, j) if (iend*jend > LATTICEZK_MATDOT_THRESHOLD)
#endif
  	{
#if defined(_OPENMP)
		#pragma omp for schedule(static)
#endif
		for (i = 0; i < iend; i++) {
			for (j = 0; j < jend; j++) {
				t(i, j) = a(i, j);
			}
		}
	}
	return true;
}

//template <typename T, typename Order1, typename Order2, typename Order3>
//bool MatrixMultiply(const Matrix<T, Order1> &a, const Matrix<T, Order2> &b, Matrix<T, Order3> &c)
template<typename T>
bool MatrixMultiply(const Matrix<T, RowMajorOrder> &a, const Matrix<T, ColumnMajorOrder> &b, Matrix<T, ColumnMajorOrder> &c)
{
	if (a.NumRows() != c.NumRows() || b.NumCols() != c.NumCols() || a.NumCols() != b.NumRows()) {
		return false;
	}
	LATTICEZK_LOG("matrix mult size: " << a.NumRows() << " | " << a.NumCols() << " | " << b.NumCols());
	c.Zero();
	matdim_t i, j, k, iend = c.NumRows(), jend = c.NumCols(), kend = a.NumCols();
#if defined(_OPENMP)
	#pragma omp parallel shared(a, b, c) private(i, j, k) if (iend > LATTICEZK_MATMUL_THRESHOLD1 && iend*jend > LATTICEZK_MATMUL_THRESHOLD2)
#endif
  	{
#if defined(_OPENMP)
		#pragma omp for schedule(static)
#endif
		for (j = 0; j < jend; j++) {
			for (i = 0; i < iend; i++) {
				T s = 0;
#if defined(_OPENMP)
	#if !defined(_WIN32)
				#pragma omp simd reduction(+:s)
	#endif
#endif
				for (k = 0; k < kend; k++) {
					s += a(i, k) * b(k, j);
				}
				c(i, j) += s;
			}
		}
	}
	return true;
}

template <typename T, typename Order>
bool MatrixAdd(const Matrix<T, Order> &a, const Matrix<T, Order> &b, Matrix<T, Order> &c)
{
	if (a.NumRows() != b.NumRows() || a.NumRows() != c.NumRows() || a.NumCols() != b.NumCols() || a.NumCols() != c.NumCols()) {
		return false;
	}
	matdim_t i, iend = c.NumCells();
	const T * adata = a.Data();
	const T * bdata = b.Data();
	T * cdata = c.Data();
#if defined(_OPENMP)
	#pragma omp simd
#endif
	for (i = 0; i < iend; i++) {
		cdata[i] = adata[i] + bdata[i];
	}
	return true;
}

template<typename T, typename Order>
bool MatrixFrobeniusInnerProduct(const Matrix<T, Order> &a, const Matrix<T, Order> &b, double &c)
{
	if (a.NumRows() != b.NumRows() || a.NumCols() != b.NumCols()) {
		return false;
	}
	matdim_t i, iend = a.NumCells();
	const T * adata = a.Data();
	const T * bdata = b.Data();
	double r = 0.0;
#if defined(_OPENMP)
	#pragma omp parallel for reduction(+:r) if (iend > LATTICEZK_MATDOT_THRESHOLD)
#endif
	for (i = 0; i < iend; i++) {
		r += (double)adata[i] * (double)bdata[i];
	}
	c = r;
	return true;
}

template<typename T, typename Order>
bool MatrixFrobeniusNorm(const Matrix<T, Order> &a, const Matrix<T, Order> &b, double &c)
{
	if (MatrixFrobeniusInnerProduct(a, b, c)) {
		c = sqrt(c);
		return true;
	}
	return false;
}

template <typename T, typename Order = RowMajorOrder>
class Matrix
{
public:
	typedef T data_t;
private:
	const matdim_t n_rows, n_cols;
	const Order order;
	data_t *data;
private:
	Matrix(const Matrix & other) = delete;
	Matrix(const Matrix && other) = delete;
public:
	Matrix(matdim_t n_rows, matdim_t n_cols) :
		n_rows(n_rows), n_cols(n_cols), order(n_rows, n_cols)
	{
		data = (data_t *)LATTICEZK_ALIGNED_ALLOC(LATTICEZK_ALIGNMENT, n_rows * n_cols * sizeof(data_t));
		Zero();
	}
	~Matrix()
	{
		LATTICEZK_ALIGNED_FREE(data);
	}
public:
	matdim_t NumRows() const
	{
		return n_rows;
	}
	matdim_t NumCols() const
	{
		return n_cols;
	}
	matdim_t NumCells() const
	{
		return n_rows * n_cols;
	}
public:
	data_t * Data()
	{
		return data;
	}
	const data_t * Data() const
	{
		return data;
	}
	void Zero()
	{
#if defined(_OPENMP)
		matdim_t i, iend = NumCells();
		if (iend > LATTICEZK_MATDOT_THRESHOLD) {
			#pragma omp parallel for private(i) schedule(static)
			for (i = 0; i < iend; i++) {
				data[i] = 0;
			}
		} else
#endif
		memset(data, 0, NumCells() * sizeof(data_t));
	}
	const data_t& operator()(matdim_t i) const
	{
		return data[order(i)];
	}
	data_t& operator()(matdim_t i)
	{
		return data[order(i)];
	}
	const data_t& operator()(matdim_t i, matdim_t j) const
	{
		return data[order(i, j)];
	}
	data_t& operator()(matdim_t i, matdim_t j)
	{
		return data[order(i, j)];
	}
public:
	bool operator==(const Matrix<T, Order> & mat) const
	{
		if (n_rows != mat.NumRows() || n_cols != mat.NumCols()) {
			return false;
		}
		const data_t * matdata = mat.Data();
		matdim_t n = NumCells();
		for (matdim_t i=0; i<n; i++) {
			if (data[i] != matdata[i]) {
				return false;
			}
		}
		return true;
	}
	bool operator!=(const Matrix<T, Order> & mat) const
	{
		return !(*this == mat);
	}
public:
	double UpperBoundOnOperatorNorm()
	{
		double r = 0;
#if defined(_OPENMP)
	#if !defined(_WIN32)
		#pragma omp parallel
	#endif
#endif
		{
#if defined(_OPENMP)
	#if !defined(_WIN32)
			#pragma omp for simd reduction(max:r)
	#else
			#pragma omp parallel for
	#endif
#endif
			for (matdim_t i=0; i<n_rows; i++) {
				double s = 0;
#if defined(_OPENMP)
	#if !defined(_WIN32)
				#pragma omp ordered simd
	#endif
#endif
				for (matdim_t j=0; j<n_cols; j++) {
					double v = (double)this->operator()(i, j);
					s += v < 0 ? -v : v;
				}
				if (r < s) {
					r = s;
				}
			}
		}
		return r;
	}
public:
	bool Multiply(const Matrix<T, Order> &a, const Matrix<T, Order> &b)
	{
		return MatrixMultiply(a, b, *this);
	}
	bool Add(const Matrix<T, Order> &a, const Matrix<T, Order> &b)
	{
		return MatrixAdd(a, b, *this);
	}
	bool FrobeniusInnerProduct(const Matrix<T, Order> &a, double &c)
	{
		return MatrixFrobeniusInnerProduct(a, *this, c);
	}
	bool FrobeniusNorm(const Matrix<T, Order> &a, double &c)
	{
		return MatrixFrobeniusNorm(a, *this, c);
	}
	bool FrobeniusNorm(double &c)
	{
		return MatrixFrobeniusNorm(*this, *this, c);
	}
};

} // namespace LatticeZK


#endif // __LATTICEZK_MATRIX_HPP_
