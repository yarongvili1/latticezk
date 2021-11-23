#ifndef __LATTICEZK_MATMULT_HPP_
#define __LATTICEZK_MATMULT_HPP_

// Matrix multiplication functions
// The functions accept all-RMO (row-major-order) or all-CMO (column-major-order) matrices
// They build on matrtix multiplication of the form (RMO,CMO) -> CMO
// This form is supported efficiently on both CPU and GPU

#include "latticezk/matrix.hpp"

namespace LatticeZK {

template<typename T>
bool MatrixMultiply(const Matrix<T, RowMajorOrder> &a, const Matrix<T, RowMajorOrder> &b, Matrix<T, RowMajorOrder> &c)
{
	Matrix<T, ColumnMajorOrder> b1(b.NumRows(), b.NumCols());
	Matrix<T, ColumnMajorOrder> c1(c.NumRows(), c.NumCols());
	return MatrixToColumnMajorOrder(b, b1) && MatrixMultiply(a, b1, c1) && MatrixToRowMajorOrder(c1, c);
}

template<typename T>
bool MatrixMultiply(const Matrix<T, ColumnMajorOrder> &a, const Matrix<T, ColumnMajorOrder> &b, Matrix<T, ColumnMajorOrder> &c)
{
	Matrix<T, RowMajorOrder> a1(a.NumRows(), a.NumCols());
	return MatrixToRowMajorOrder(a, a1) && MatrixMultiply(a1, b, c);
}

} // namespace LatticeZK

#endif // __LATTICEZK_MATMULT_HPP_
