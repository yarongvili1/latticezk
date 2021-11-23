#ifndef __LATTICEZK_MATRIXOPS_HPP_
#define __LATTICEZK_MATRIXOPS_HPP_

// Matrix operations for CPU
//   - copying a matrix to another of the same major-ordering
//   - syncing a matrix after it has been modified
//   - matrix multiplication in the form (RMO,CMO) -> CMO
// Different implementations of the same operations are available for GPU code

#include "latticezk/matrix.hpp"

namespace LatticeZK {

template<typename T>
class MatrixOps
{
public:
	typedef T data_t;
	typedef Matrix<T, RowMajorOrder> RowMajorMatrix;
	typedef Matrix<T, ColumnMajorOrder> ColumnMajorMatrix;
public:
	MatrixOps()
	{
	}
public:
	template<typename Order>
    bool Copy(Matrix<T, Order>& dst, const Matrix<T, Order>& src)
    {
		if (src.NumRows() != dst.NumRows() || src.NumCols() != dst.NumCols()) {
			return false;
		}
		data_t* dstdata = dst.Data();
		const data_t* srcdata = src.Data();
		matdim_t n = dst.NumCells();
		matdim_t i;
#if defined(_OPENMP)
		#pragma omp parallel for if (n > LATTICEZK_MATDOT_THRESHOLD)
#endif
		for (i = 0; i < n; i++) {
			dstdata[i] = srcdata[i];
		}
		return true;
    }
	bool Sync(RowMajorMatrix & mat)
	{
		LATTICEZK_UNUSED(mat);
		return true;
	}
	bool Sync(ColumnMajorMatrix & mat)
	{
		LATTICEZK_UNUSED(mat);
		return true;
	}
	bool Multiply(const RowMajorMatrix &a, const ColumnMajorMatrix &b, ColumnMajorMatrix &c)
	{
		return MatrixMultiply(a, b, c);
	}
};

} // namespace LatticeZK

#endif // __LATTICEZK_MATRIXOPS_HPP_
