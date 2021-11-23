#ifndef __LATTICEZK_MATRIXEXT_HPP_
#define __LATTICEZK_MATRIXEXT_HPP_

// Extended matrix operations that are only used debugging
// These operations are designed for CPU but not GPU code

#include <iostream>
#include "Eigen/Dense"
#include "latticezk/util/cpucycles.hpp"
#include "latticezk/matrix.hpp"

namespace LatticeZK {

#define LATTICEZK_COPY_MATRIX_TO_EIGEN(T, mat, emat) \
	Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> emat(0, 0); \
	{ \
		uint32_t n_rows = mat.NumRows(), n_cols = mat.NumCols(); \
		emat.resize(n_rows, n_cols); \
		for (uint32_t i=0; i<n_rows; i++) { \
			for (uint32_t j=0; j<n_cols; j++) { \
				emat(i, j) = mat(i, j); \
			} \
		} \
	}


template <typename T, typename Order>
double OperatorNorm(const Matrix<T, Order> & mat)
{
	LATTICEZK_COPY_MATRIX_TO_EIGEN(T, mat, emat);
	uint64_t t0 = cpucycles();
	double r = emat.OperatorNorm();
	uint64_t t1 = cpucycles();
	std::cerr << "done computing s1=" << r << " cycles=" << (t1 - t0) << " bound=" << mat.UpperBoundOnOperatorNorm() << std::endl;
	return r;
}


template <typename T, typename Order>
std::ostream & operator<<(std::ostream & os, const Matrix<T, Order> & mat)
{
	LATTICEZK_COPY_MATRIX_TO_EIGEN(T, mat, emat);
	return os << emat;
}

} // namespace LatticeZK

#endif // __LATTICEZK_MATRIXEXT_HPP_
