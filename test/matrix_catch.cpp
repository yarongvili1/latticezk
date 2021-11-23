#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <catch2/catch.hpp>
#include "latticezk/matrix.hpp"
#include "latticezk/matmult.hpp"
#include "testcommon.h"

namespace LatticeZK {

typedef Matrix<int32_t> Matrix32s;

void test_1x1_matrix32_multiplication(int32_t a, int32_t b) {
	int32_t c = a * b;
	Matrix32s aM(1, 1), bM(1, 1), cM(1, 1);
	aM(0, 0) = a;
	bM(0, 0) = b;
	cM(0, 0) = 0;
	REQUIRE( cM(0, 0) != c );
	REQUIRE( cM.Multiply(aM, bM) );
	REQUIRE( aM(0, 0) == a );
	REQUIRE( bM(0, 0) == b );
	REQUIRE( cM(0, 0) == c );
}

void test_1x2_matrix32_multiplication(int32_t a0, int a1, int b0, int b1) {
	int32_t c = a0 * b0 + a1 * b1;
	Matrix32s aM(1, 2), bM(2, 1), cM(1, 1);
	aM(0, 0) = a0;
	aM(0, 1) = a1;
	bM(0, 0) = b0;
	bM(1, 0) = b1;
	cM(0, 0) = 0;
	REQUIRE( cM(0, 0) != c );
	REQUIRE( cM.Multiply(aM, bM) );
	REQUIRE( aM(0, 0) == a0 );
	REQUIRE( aM(0, 1) == a1 );
	REQUIRE( bM(0, 0) == b0 );
	REQUIRE( bM(1, 0) == b1 );
	REQUIRE( cM(0, 0) == c );
}

void test_2x1_matrix32_multiplication(int32_t a0, int a1, int b0, int b1) {
	int32_t c[] = { a0 * b0, a0 * b1, a1 * b0, a1 * b1 };
	Matrix32s aM(2, 1), bM(1, 2), cM(2, 2);
	aM(0, 0) = a0;
	aM(1, 0) = a1;
	bM(0, 0) = b0;
	bM(0, 1) = b1;
	for (int i=0; i<2; i++) {
		CAPTURE( i );
		for (int j=0; j<2; j++) {
			CAPTURE( j );
			cM(i, j) = 0;
			REQUIRE( cM(i, j) != c[i*2+j] );
		}
	}
	REQUIRE( cM.Multiply(aM, bM) );
	REQUIRE( aM(0, 0) == a0 );
	REQUIRE( aM(0, 1) == a1 );
	REQUIRE( bM(0, 0) == b0 );
	REQUIRE( bM(0, 1) == b1 );
	for (int i=0; i<2; i++) {
		CAPTURE( i );
		for (int j=0; j<2; j++) {
			CAPTURE( j );
			REQUIRE( cM(i, j) == c[i*2+j] );
		}
	}
}

void test_mxnxk_matrix32_multiplication(int m, int k, int n, unsigned int seed) {
	srand(seed);
	Matrix32s aM(m, k), bM(k, n), cM(m, n), cX(m, n);
	for (int i=0; i<m; i++) {
		for (int j=0; j<k; j++) {
			aM(i) = (int)rand();
		}
	}
	for (int i=0; i<k; i++) {
		for (int j=0; j<n; j++) {
			bM(i) = (int)rand();
		}
	}
	cM.Zero();
	uint64_t cycles0 = rdtsc_start();
	REQUIRE( cM.Multiply(aM, bM) );
	uint64_t cycles1 = rdtsc_stop();
	std::cerr << m << "x" << n << "x" << k << " cycles: " << (cycles1 - cycles0) << std::endl;
	for (int i1=0; i1<m; i1++) {
		for (int i2=0; i2<n; i2++) {
			int s = 0;
			for (int i3=0; i3<k; i3++) {
				s += aM(i1, i3) * bM(i3, i2);
			}
			cX(i1, i2) = s;
		}
	}
	for (int i1=0; i1<m; i1++) {
		for (int i2=0; i2<n; i2++) {
			REQUIRE( cM(i1, i2) == cX(i1, i2) );
		}
	}
}

TEST_CASE( "1x1 matrix32s multiplication is sane", "[latticezk]" ) {
	int32_t a[] = {11, 0x1234, 0x7fff, 0x27fff, 0x76543210};
	int32_t b[] = {13, 0x5678, 0x7fff, 0x27fff, 0x01234567};
	for (size_t i=0; i<sizeof(a)/sizeof(a[0]); i++) {
		CAPTURE( i );
		test_1x1_matrix32_multiplication(a[i], b[i]);
	}
}

TEST_CASE( "1x2 matrix32s multiplication is sane", "[latticezk]" ) {
	int32_t a0[] = {11, 0x1234, 0x7fff, 0x27fff, 0x76543210};
	int32_t a1[] = {11, 0x1234, 0x7fff, 0x27fff, 0x76543210};
	int32_t b0[] = {13, 0x5678, 0x7fff, 0x27fff, 0x01234567};
	int32_t b1[] = {13, 0x5678, 0x7fff, 0x27fff, 0x01234567};
	for (size_t i=0; i<sizeof(a0)/sizeof(a0[0]); i++) {
		CAPTURE( i );
		test_1x2_matrix32_multiplication(a0[i], a1[i], b0[i], b1[i]);
	}
}

TEST_CASE( "2x1 matrix32s multiplication is sane", "[latticezk]" ) {
	int32_t a0[] = {11, 0x1234, 0x7fff, 0x27fff, 0x76543210, 0x6b8b4567};
	int32_t a1[] = {11, 0x3412, 0x7f11, 0x27f11, 0x76325410, 0x327b23c6};
	int32_t b0[] = {13, 0x5678, 0x7fff, 0x27fff, 0x01234567, 0x643c9869};
	int32_t b1[] = {13, 0x7856, 0x7f11, 0x27f11, 0x01452367, 0x66334873};
	for (size_t i=0; i<sizeof(a0)/sizeof(a0[0]); i++) {
		CAPTURE( i );
		test_2x1_matrix32_multiplication(a0[i], a1[i], b0[i], b1[i]);
	}
}

TEST_CASE( "mxkxn matrix32s multiplication is sane", "[latticezk]" ) {
	test_mxnxk_matrix32_multiplication(2, 1, 2, 1);
	test_mxnxk_matrix32_multiplication(3, 1, 3, 1);
	test_mxnxk_matrix32_multiplication(100, 100, 100, 1);
	test_mxnxk_matrix32_multiplication(1000, 1000, 1000, 1);
}

void test_mxn_matrix32_addition(int m, int n, unsigned int seed) {
	srand(seed);
	Matrix32s aM(m, n), bM(m, n), cM(m, n), cX(m, n);
	for (int i=0; i<m*n; i++) {
		aM(i) = (int)rand();
		bM(i) = (int)rand();
		cX(i) = aM(i) + bM(i);
	}
	for (int i=0; i<m*n; i++) {
		REQUIRE( cX(i) != cM(i) );
	}
	REQUIRE( cM.Add(aM, bM) );
	for (int i=0; i<m*n; i++) {
		REQUIRE( cX(i) == cM(i) );
	}
}

TEST_CASE( "m*n matrix32s addition is sane", "[latticezk]" ) {
	test_mxn_matrix32_addition(2, 1, 1);
	test_mxn_matrix32_addition(1, 2, 1);
	test_mxn_matrix32_addition(2, 2, 1);
	test_mxn_matrix32_addition(100, 100, 1);
}

} // namespace LatticeZK
