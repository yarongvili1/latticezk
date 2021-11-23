#ifndef __LATTICEZK_PROVER_HPP_
#define __LATTICEZK_PROVER_HPP_

// the core implementation of the Lattice-based NIZK protocol

#include <cmath>
#include <random>
#include "crypto/hasher/sha.h"
#include "crypto/number.h"
#include "latticezk/matrix.hpp"
#include "latticezk/gaussian/gsampler.hpp"
#include "latticezk/log.hpp"

namespace LatticeZK
{

// hash a matrix using SHA-256
// the hash is used to seed AES in CTR mode
class MatrixHasher
{
private:
	crypto::hasher::SHA<256> sha256;
public:
	MatrixHasher()
	{
	}
public:
	template<typename T, typename Order>
	void Update(const Matrix<T, Order> & mat)
	{
		sha256.update(mat.Data(), mat.NumCells() * sizeof(T));
	}
	void Digest(AES_Random & aes_rnd)
	{
		crypto::Number<256, uint8_t> digest = sha256.digest();
		uint8_t * data = digest.data();
		uint8_t seed[16];
		for (int i=0; i<16; i++) {
			seed[i] = data[i];
		}
		data += 16;
		for (int i=0; i<16; i++) {
			seed[i] ^= data[i];
		}
		aes_rnd.reseed(seed);
	}
};

// Wrap AES in CTR mode as a 64-bit random_engine
class AES_random_engine
{
public:
	typedef uint64_t result_type;
	static constexpr result_type min() { return 0; };
	static constexpr result_type max() { return 0xFFFFFFFFFFFFFFFFLL; };
private:
	U64Sampler usampler;
public:
	AES_random_engine(AES_Random & aes_rnd) :
		usampler(aes_rnd)
	{
	}
public:
	inline result_type operator()()
	{
		return usampler();
	}
};

// Uniform probability sampling using AES in CTR mode as the randomness source
class ProbabilitySampler
{
private:
	static constexpr uint64_t mantissa_mask = ((((uint64_t)1) << 52) - 1);
private:
	AES_random_engine engine;
	std::uniform_real_distribution<double> udist;
public:
	ProbabilitySampler(AES_Random & aes_rnd) :
		engine(aes_rnd), udist(0.0, 1.0)
	{
	}
public:
	inline double operator()()
	{
		return udist(engine);
	}
};

// Uniform matrix sampling using various supported randomness sources
template<typename Sampler>
class MatrixSampler
{
private:
	Sampler sampler;
public:
	MatrixSampler(AES_Random & aes_rnd) :
		sampler(aes_rnd)
	{
	}
	MatrixSampler(BytesSampler& bytes_sampler) :
		sampler(bytes_sampler)
	{
	}
	MatrixSampler(Sampler & sampler) :
		sampler(sampler)
	{
	}
public:
	template<typename T, typename Order>
	inline bool operator()(Matrix<T, Order> &mat)
	{
		matdim_t n = mat.NumCells();
		T * data = mat.Data(); 
		for (matdim_t i=0; i<n; i++) {
			data[i] = (T)sampler();
		}
		return true;
	}
};

// Check a matrix is consistent with a randomness source
template<typename Sampler>
class MatrixChecker
{
private:
	Sampler sampler;
public:
	MatrixChecker(AES_Random & aes_rnd) :
		sampler(aes_rnd)
	{
	}
	MatrixChecker(Sampler sampler) :
		sampler(sampler)
	{
	}
public:
	template<typename T, typename Order>
	inline bool operator()(const Matrix<T, Order> &mat)
	{
		matdim_t n = mat.NumCells();
		const T * data = mat.Data();
		for (matdim_t i=0; i<n; i++) {
			if (data[i] != (T)sampler()) {
				return false;
			}
		}
		return true;
	}
};

// Implementation of rejection sampling in the protocol
class Reject
{
private:
	ProbabilitySampler sampler;
	double inner_denom, outer_denom;
public:
	Reject(AES_Random & aes_rnd, double sigma, double rho) :
		sampler(aes_rnd), inner_denom(1.0 / (2 * sigma * sigma)), outer_denom(1.0 / rho)
	{
	}
public:
	template<typename T, typename Order>
	bool operator()(Matrix<T, Order> & mat_Z, Matrix<T, Order> & mat_B)
	{
		double u = sampler(), ZB, BB;
		return mat_Z.FrobeniusInnerProduct(mat_B, ZB) && mat_B.FrobeniusNorm(BB) && u <= exp( (-2 * ZB + BB) * inner_denom ) * outer_denom;
	}
};

// Captures a proof in the protocol
template<typename T, typename MatOps>
class Proof
{
public:
	typedef T data_t;
	typedef typename MatOps::RowMajorMatrix RowMajorMatrix;
	typedef typename MatOps::ColumnMajorMatrix ColumnMajorMatrix;
public:
	const matdim_t r, v, l, n;
	const double B;
	RowMajorMatrix mat_A, mat_T;
	ColumnMajorMatrix mat_W, mat_C, mat_Z;
public:
	Proof(matdim_t r, matdim_t v, matdim_t l, matdim_t n, double B) :
		r(r), v(v), l(l), n(n), B(B),
		mat_A(r, v), mat_T(r, l), mat_W(r, n), mat_C(l, n), mat_Z(v, n)
	{
	}

	bool seed(AES_Random& aes_rnd)
	{
		MatrixHasher mathasher;
		mathasher.Update(mat_A);
		mathasher.Update(mat_T);
		mathasher.Update(mat_W);
		mathasher.Digest(aes_rnd);
		return true;
	}
};

// Implementation of the prover in the protocol
template<typename T, typename G, typename MatOps>
class Prover
{
public:
	typedef T data_t;
	typedef G gsampler_t;
	typedef Proof<T, MatOps> proof_t;
	typedef typename MatOps::RowMajorMatrix RowMajorMatrix;
	typedef typename MatOps::ColumnMajorMatrix ColumnMajorMatrix;
	typedef MatrixSampler<BitSampler> BinaryMatrixSampler;
	typedef MatrixSampler<G> GaussianMatrixSampler;
private:
	MatOps matops;
	matdim_t r, v, l, n;
	double sigma, rho, B;
	RowMajorMatrix mat_A;
	ColumnMajorMatrix mat_S;
	RowMajorMatrix lmat_S, lmat_T;
	ColumnMajorMatrix mat_T, mat_Y, mat_W, mat_C, mat_B, mat_Z; // column-major-order fits right-multiplication and its result matrices
private:
	// the copy- and move-constructors are private to prevent passing-by-value
	Prover(const Prover & other) = delete;
	Prover(const Prover && other) = delete;
private:
	// the main constructor is private so that parameter-checking can be enforced before it is invoked
	Prover(MatOps & matops, RowMajorMatrix &matA, ColumnMajorMatrix &matS, matdim_t n, double rho, double B) :
		matops(matops), r(matA.NumRows()), v(matA.NumCols()), l(matS.NumCols()), n(n), sigma(gsampler_t::sigma), rho(rho), B(B),
		mat_A(r, v), mat_S(v, l), lmat_S(v, l), lmat_T(r, l), mat_T(r, l), mat_Y(v, n), mat_W(r, n), mat_C(l, n), mat_B(v, n), mat_Z(v, n)
	{
		bool success = true;
		LATTICEZK_TIME(success, matops.Copy(mat_A, matA), "copying A");
		LATTICEZK_TIME(success, matops.Copy(mat_S, matS), "copying S");
		LATTICEZK_TIME(success, matops.Sync(mat_A), "syncing A");
		LATTICEZK_TIME(success, matops.Sync(mat_S), "syncing S");
		LATTICEZK_TIME(success, matops.Multiply(mat_A, mat_S, mat_T), "multiplying A*S");
		LATTICEZK_TIME(success, MatrixToRowMajorOrder(mat_S, lmat_S), "reordering S");
		LATTICEZK_TIME(success, matops.Sync(lmat_S), "syncing S");
		LATTICEZK_TIME(success, MatrixToRowMajorOrder(mat_T, lmat_T), "reordering T");
		LATTICEZK_TIME(success, matops.Sync(lmat_T), "syncing T");
	}
public:
	// Create a prover with given parameters, but return nullptr if parameter-checking failed
	static Prover * Create(MatOps & matops, RowMajorMatrix & matA, ColumnMajorMatrix & matS, uint32_t lambda, double s, matdim_t n, double rho)
	{
		if (matA.NumCols() != matS.NumRows() || n < 0 || (uint32_t)n < lambda + 2 || rho <= 1.0) {
			LATTICEZK_LOG("prover creation failed (1): " << (matA.NumCols() != matS.NumRows()) << " " << (n < 0) << " " << ((uint32_t)n < lambda + 2) << " " << (rho <= 1.0));
			return nullptr;
		}
		matdim_t l = matS.NumCols();
		LATTICEZK_LOG("operator norm upper bound=" << matS.UpperBoundOnOperatorNorm() << " s=" << s);
		LATTICEZK_LOG("sigma=" << gsampler_t::sigma << " required>=" << (12 / log(rho) * s * sqrt(l*n)));
		double s1 = matS.UpperBoundOnOperatorNorm();
		if (s <= 0 || s1 > s || gsampler_t::sigma < 12 / log(rho) * s * sqrt(l*n)) {
			LATTICEZK_LOG("prover creation failed (2): " << (s <= 0) << " " << (s1 > s) << " " << (gsampler_t::sigma < 12 / log(rho) * s * sqrt(l*n)));
			return nullptr;
		}
		matdim_t v = matA.NumCols();
		double B = sqrt(2*v) * gsampler_t::sigma;
		return new Prover(matops, matA, matS, n, rho, B);
	}
public:
	double GetB() const
	{
		return B;
	}
public:
	bool Commit(AES_Random & aes_rnd, proof_t &proof)
	{
		bool success = true;
		BytesSampler bytes_sampler(aes_rnd);
		GaussianMatrixSampler gsampler(bytes_sampler);
		LATTICEZK_TIME(success, gsampler(mat_Y), "sampling Y");
		LATTICEZK_TIME(success, matops.Sync(mat_Y), "syncing Y");
		LATTICEZK_TIME(success, matops.Multiply(mat_A, mat_Y, mat_W), "multiplying A*Y");
		LATTICEZK_TIME(success, matops.Copy(proof.mat_A, mat_A), "copying A to proof");
		LATTICEZK_TIME(success, matops.Copy(proof.mat_T, lmat_T), "copying T to proof");
		LATTICEZK_TIME(success, matops.Copy(proof.mat_W, mat_W), "copying W to proof");
		return success;
	}
	bool Challenge(proof_t &proof)
	{
		bool success = true;
		AES_Random aes_rnd;
		LATTICEZK_TIME(success, proof.seed(aes_rnd), "seeding");
		BinaryMatrixSampler bsampler(aes_rnd);
		LATTICEZK_TIME(success, bsampler(mat_C), "sampling C");
		LATTICEZK_TIME(success, matops.Sync(mat_C), "syncing C");
		LATTICEZK_TIME(success, matops.Copy(proof.mat_C, mat_C), "copying C to proof");
		return success;
	}
	bool Response(proof_t &proof)
	{
		bool success = true;
		LATTICEZK_TIME(success, matops.Multiply(lmat_S, mat_C, mat_B), "multiplying S*C");
		mat_Z.Add(mat_B, mat_Y);
		LATTICEZK_TIME(success, matops.Copy(proof.mat_Z, mat_Z), "copying Z to proof");
		return success;
	}
	uint64_t Prove(AES_Random & aes_rnd, proof_t &proof)
	{
		uint64_t draws = 0;
		Reject reject(aes_rnd, sigma, rho);
		do {
			++draws;
			LATTICEZK_LOG("draw=" << draws);
			Commit(aes_rnd, proof);
			Challenge(proof);
			Response(proof);
		} while (reject(mat_Z, mat_B));
		return draws;
	}
};

// Implementation of the verifier in the protocol
template<typename T, typename MatOps>
class Verifier
{
public:
	typedef T data_t;
	typedef Proof<T, MatOps> proof_t;
	typedef typename MatOps::RowMajorMatrix RowMajorMatrix;
	typedef typename MatOps::ColumnMajorMatrix ColumnMajorMatrix;
	typedef MatrixChecker<BitSampler> BinaryMatrixChecker;

private:
	MatOps & matops;
	matdim_t r, v, l, n;
	double B;
	ColumnMajorMatrix mat_AZ, mat_TC, mat_TCpW; // column-major-order fits right-multiplication and its result matrices
	Matrix<double> mat_Zcols;
private:
	// the copy- and move-constructors are private to prevent passing-by-value
	Verifier(const Verifier & other) = delete;
	Verifier(const Verifier && other) = delete;
public:
	Verifier(MatOps & matops, matdim_t r, matdim_t v, matdim_t l, matdim_t n, double B) :
		matops(matops), r(r), v(v), l(l), n(n), B(B),
		mat_AZ(r, n), mat_TC(r, n), mat_TCpW(r, n), mat_Zcols(1, n)
	{
	}
public:
	bool Verify(proof_t &proof)
	{
		if (B < proof.B
				|| proof.mat_A.NumRows() != r || proof.mat_A.NumCols() != v
				|| proof.mat_Z.NumRows() != v || proof.mat_Z.NumCols() != n
				|| proof.mat_W.NumRows() != r || proof.mat_W.NumCols() != n
				|| proof.mat_T.NumRows() != r || proof.mat_T.NumCols() != l
				|| proof.mat_C.NumRows() != l || proof.mat_C.NumCols() != n) {
			LATTICEZK_LOG("verification failed: mismatching dimensions");
			return false;
		}
		// TODO: sync A, Z, T, C
		bool success = true;
		AES_Random aes_rnd;
		LATTICEZK_TIME(success, proof.seed(aes_rnd), "seeding");
		if (!success) {
			LATTICEZK_LOG("verification failed: seeding");
			return false;
		}
		BinaryMatrixChecker bchecker(aes_rnd);
		if (!bchecker(proof.mat_C)) {
			LATTICEZK_LOG("verification failed: mismatching challenge matrix");
			return false;
		}
		LATTICEZK_LOG("multiplying A*Z and T*C");
		if (!matops.Multiply(proof.mat_A, proof.mat_Z, mat_AZ)
			|| !matops.Multiply(proof.mat_T, proof.mat_C, mat_TC)
			|| !mat_TCpW.Add(mat_TC, proof.mat_W))
		{
			LATTICEZK_LOG("verification failed: calculating matrices");
			return false;
		}
		if (mat_AZ != mat_TCpW) {
			LATTICEZK_LOG("verification failed: A*Z = T*C + W");
			return false;
		}
		mat_Zcols.Zero();
		for (matdim_t i=0; i<v; i++) {
			for (matdim_t j=0; j<n; j++) {
				double z = (double)((proof.mat_Z)(i, j));
				mat_Zcols(j) += z * z;
			}
		}
		for (matdim_t j=0; j<n; j++) {
			if (mat_Zcols(j) > B * B) {
				LATTICEZK_LOG("verification failed: norm-bound exceeded: B^2=" << (B*B) << " ||z_j||_2^2=" << mat_Zcols(j) << " j=" << j);
				return false;
			}
		}
		return true;
	}
};

} // namespace LatticeZK

#endif // __LATTICEZK_PROVER_HPP_
