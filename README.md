# LatticeZK

This is an experimental research code repository, provided under the terms of
the accompanying license, that implements a protocol introduced in the paper
"Sub-Linear Lattice-Based Zero-Knowledge Arguments for Arithmetic Circuits" by
Baum et al, for modulus `2^w` where `w` in `{8,16,32}`, along with techniques
from the papers:
- "Pushing the speed limit of constant-time discrete Gaussian sampling: A case
  study on Falcon" by Karmakar et al;
- "FACCT: FAst, compact, and constant-time discrete Gaussian sampler over
  Integers" by Zhao et al;
- "Faster Matrix-Vector Multiplication on GeForce 8800GTX" by Noriyuki Fujimoto.
as well as original enhancements.

The code supports running certain parts of the implementation in parallel using
AVX and OpenMP on the CPU as well as using CUDA on the GPU. The code includes
facilities for:

1. Fast constant-time (pseudorandom) uniform-sampling using AES in CTR-mode as
   well as using CURAND on the GPU.
2. Fast constant-time Gaussian-sampling using (code-generated) Boolean-logic,
   bit-slicing, and AVX for specific sets of Gaussian parameters.
3. Fast constant-time Gaussian-sampling using the Facct algorithm (via Bernouli
   sampling and floating-point manipulation) for a wide range of sigma (standard
   deviation) values using AVX as well as using CUDA on the GPU.
4. Fast parallel matrix-multiplication over `Z\_{2^w}` for `w` in `{8,16,32}`
   using OpenMP as well as using CUDA on the GPU.

The code was manually tested on the following platforms:

- *Linux*: Ubuntu 20.04, gcc 9.3.0-17ubuntu1~20.04
- *Windows*: Windows 10, Microsoft Visual Studio 2019, MSVC 19.29.30136, CUDA
  11.3, NVCC 11.3.109

The CUDA parts of the code were manually tested on Windows only.

Summary of performance results (described in more details below) on an Intel(R)
Core(TM) i7-10875H CPU @ 2.30GHz CPU and on an NVIDIA GeForce RTX 2060 (with
1920 CUDA cores):

- ZK protocol (see below for parameters): 1.3 seconds single-threaded on CPU,
  0.6 seconds multi-threaded on CPU.
- Uniform-sampling: 0.42 cycles/bytes on CPU.
- Gaussian-sampling using Boolean logic:
-- 175M samples/sec for `sigma=215` on CPU.
-- 44M samples/sec for `sigma=215` plus parameter adjustment on CPU.
- Gaussian-sampling using Facct:
-- 52M samples/sec for `sigma=215` on CPU.
-- 32M samples/sec for `sigma=10^6` on CPU.
-- 8M samples/sec for `sigma=2*10^9` on CPU.
-- 270M samples/sec for `sigma=215` on GPU.
-- 260M samples/sec for `sigma=10^6` on GPU.
-- 110M samples/sec for `sigma=2*10^9` on GPU.

## Dependencies of the code

The code uses these dependencies as submodules:
1. [Eigen library](https://gitlab.com/libeigen/eigen)
2. [gcem library](https://github.com/kthohr/gcem)
3. [quasis-crypto](https://github.com/yarongvili1/quasis-crypto)

After cloining this repository, initialize these submodules using:
```
git submodule update --init --recursive
```

Note that the dependency on Eigen is not mandatory for the implementation of the
protocol. It is only needed if one wishes to compute the operator norm exactly,
in place of the upper bound on the operator norm currently being computed.

## Building the code

Building single-threaded on Linux:
```
mkdir -p build/release
cd build/release
cmake ../..
make
```

Building multi-threaded on Linux:
```
mkdir -p build/release-omp
cd build/release-omp
cmake ../.. -DLATTICEZK_ENABLE_OPENMP=On
make
```

On Windows, open the file "latticezk.sln" in Microsoft Visual Studio, select any
of the available build configurations, and build the project.

## Running tests

To run the available tests:
```
./test/latticezk_catch
```
Note that the tests only cover some matrix operations on the CPU.

## Running the protocol for the CPU on Linux

To run the protocol for the CPU on Linux, invoke `./src/prover`. The protocol
has a number of parameters:
- *sigma*: Standard deviation of the discrete Gaussian.
- *s_bits*: Number of bits in each entry of the secret matrix.
- *lambda*: Security parameter, in units of bits.
- *n,r,v,l*: Dimension parameters of matrices.
- *rho*: Reciprocal of the rejection probability.
Due to the use of rejection sampling, the protocol may run more than one draw
until finding a valid one.

A single-threaded-mode invocation demonstrates the protocol with the parameters
```
        constexpr uint32_t sigma = 2000000000;
        int s_bits = 7;
        uint32_t lambda = 80;
        matdim_t n = 100;
        double rho = 2;
        matdim_t r = 100, v = 3000, l = 3000;
```
completed on an Intel(R) Core(TM) i7-10875H CPU @ 2.30GHz CPU on Linux in about
1.3 seconds:
```
$ time ./src/prover 
sampling A : cycles=5755922
sampling S : cycles=79643174
operator norm upper bound=99398 s=192000
sigma=2e+09 required>=1.82061e+09
copying A : cycles=658486
copying S : cycles=19558594
syncing A : cycles=14
syncing S : cycles=14
matrix mult size: 100 | 3000 | 3000
multiplying A\*S : cycles=624640209
reordering S : cycles=203436136
syncing S : cycles=12
reordering T : cycles=3551412
syncing T : cycles=14
draw=1
sampling Y : cycles=84909635
syncing Y : cycles=12
matrix mult size: 100 | 3000 | 100
multiplying A*Y : cycles=26132472
copying A to proof : cycles=527876
copying T to proof : cycles=628496
copying W to proof : cycles=18006
seeding : cycles=48262196
sampling C : cycles=693606
syncing C : cycles=14
copying C to proof : cycles=542320
matrix mult size: 3000 | 3000 | 100
multiplying S*C : cycles=1100894758
copying Z to proof : cycles=493008
proving : cycles=1265945795
seeding : cycles=47120994
multiplying A\*Z and T\*C
matrix mult size: 100 | 3000 | 100
matrix mult size: 100 | 3000 | 100
verifying : cycles=117333312
draws=1 verified=1

real	0m1.274s
user	0m1.220s
sys	0m0.052s
```
while a multi-threaded-mode invocation demonstrates the protocol with the same
parameters completed on an Intel(R) Core(TM) i7-10875H CPU @ 2.30GHz CPU in
about 0.6 seconds:
```
$ time ./src/prover 
sampling A : cycles=5792148
sampling S : cycles=78237750
operator norm upper bound=99398 s=192000
sigma=2e+09 required>=1.82061e+09
copying A : cycles=694296
copying S : cycles=23479787
syncing A : cycles=14
syncing S : cycles=14
matrix mult size: 100 | 3000 | 3000
multiplying A\*S : cycles=298721086
reordering S : cycles=37623488
syncing S : cycles=14
reordering T : cycles=753288
syncing T : cycles=14
draw=1
sampling Y : cycles=88062526
syncing Y : cycles=14
matrix mult size: 100 | 3000 | 100
multiplying A\*Y : cycles=7971630
copying A to proof : cycles=1552354
copying T to proof : cycles=1076950
copying W to proof : cycles=33076
seeding : cycles=50109304
sampling C : cycles=713244
syncing C : cycles=14
copying C to proof : cycles=866114
matrix mult size: 3000 | 3000 | 100
multiplying S\*C : cycles=459214512
copying Z to proof : cycles=324588
proving : cycles=611499680
seeding : cycles=49589080
multiplying A\*Z and T\*C
matrix mult size: 100 | 3000 | 100
matrix mult size: 100 | 3000 | 100
verifying : cycles=95712133
draws=1 verified=1

real	0m0.575s
user	0m3.844s
sys	0m0.139s
```

## Running the protocol on Windows

To run the protocol and other experiments for the CPU and GPU on Windows, run
the project in Microsoft Visual Studio.

## Fast constant-time uniform-sampling using AES-CTR

The fast constant-time uniform-sampling using AES-CTR generates a sequence of
pseudorandom blocks of 128 bits that repeats after `2^{128}` entries. Some other
samplers that build on it repeat after somewhat fewer entries due to using a few
AES-CTR samples per sample they generate. Its speed is demonstrated by the
`src/usampler` tool running on an Intel(R) Core(TM) i7-10875H CPU @ 2.30GHz CPU
on Linux:
```
./src/usampler 
Cycles for all samples: 56687050
Number of samples: 16777216
Cycles per sample: 3.378811
Sum: 2063456674819040554
```
Being 8-bytes per sample, this translates to a rate of about 0.42 cycles/byte.

## Fast constant-time uniform-sampling using CURAND on the GPU

The CURAND library provides fast generation of pseudorandom samples on the GPU
to enable parallel consumption of these samples, they are divided into blocks.
The consumer configures the block size as well as a number `max_draws` of blocks
to allocate per thread of consumption. A consuming thread may run out of samples
in the blocks allocated, whence it should produce partially failed output. This
design calls for a configuration to find an appropriate tradeoff between the
frequency of failed output and the over-provisioning of pseudorandom samples.
See below for success rates over 0.95 obtained in Gaussian sampling on the GPU.

## Fast constant-time Gaussian-sampling using Boolean-logic

The fast constant-time Gaussian-sampling using Boolean-logic generates a
sequence of pseudoranom Guassian samples with parameters chosen at the time of
code-generation (not in this repository). Its speed is demonstrated by the
`src/gsampler` tool running on an Intel(R) Core(TM) i7-10875H CPU @ 2.30GHz CPU
on Linux:
```
./src/gsampler 
Number of rejections: 0
Cycles for all samples: 337782014
Number of samples: 25600000
Cycles per sample: 13.194610
...
```
The above runs with sigma of 215 and 10 output bits, translating to about 6.6
cycles/byte and about 175M samples/sec. A parameterized invocation of the tool
can be used to moderately adjust the parameters at some performance cost:
```
./src/gsampler 0.5 210
Number of rejections: 190800
Cycles for all samples: 1335752056
Number of samples: 25600000
Cycles per sample: 52.177815
...
```
This translates to about 26.1 cycles/byte and about 44M samples/sec.

## Fast constant-time Gaussian-sampling using the Facct algorithm using AVX

The Facct algorithm supports generating a sequence of Gaussian samples for a
range of sigma values. The original Facct code only supports sigma values up to
8-bits wide. The code in this repository has been enhanced to support sigma
values up to 32-bits wide while improving performance on the CPU and enabling
its use also on the GPU. Its speed is demonstrated for `sigma=215` by the
`src/facct` tool running on an Intel(R) Core(TM) i7-10875H CPU @ 2.30GHz CPU on
Linux:
```
./src/facct 
NHIST: 4096
Number of rejections: 0
Cycles for all samples: 1130738983
Number of samples: 25600000
Cycles per sample: 44.169492
```
This translates to about 5.52 cycles/byte and about 52M samples/sec. For
`sigma=10^6` the rate reduces to about 32M samples/sec and for `sigma=2\*10^9`
the rate reduces to about 8M samples/sec. These reductions are due to the
increase from 8- to 16- and 32-bit-wide sigma values.

## Fast constant-time Gaussian-sampling using the Facct algorithm using CUDA

When running on the GPU, the Facct algorithm runs as a CUDA kernel that consumes
pseduorandom samples, which were generated earlier on the GPU, and generates 8
samples per thread-execution. Some samples may not succeeed, as measured by the
sampling success rate, if, due to rejections in the Gaussian sampling, an
execution needs more pseudorandom samples than pre-generated for it. To port the
algorithm to the GPU, the AVX intrinsics the algorithm uses have been replaced
with their implementation on the GPU. While the GPU used in the experiments does
not support 64-bit-lane SIMD instructions that help speedup CPU execution, the
ported algorithm more than compensates for this with massive parallelization.

The first kernel invocation, for `sigma=215` on NVIDIA GeForce RTX 2060
(with 1920 CUDA cores), demonstrates a rate of about 270M samples/sec on the GPU
and a latency of about 100 msec.
```
Facct sampling kernel took 31.035839 milliseconds
8388608 samples (0.989 success rate) took 132.053 milliseconds.
```

The second kernel invocation, for `sigma=10^6` on NVIDIA GeForce RTX 2060
(with 1920 CUDA cores), demonstrates a rate of about 260M samples/sec on the GPU
and a latency of about 130 msec.
```
Facct sampling kernel took 32.229439 milliseconds
8388608 samples (0.995 success rate) took 159.278 milliseconds.
...
```

The third kernel invocation, for `sigma=2*10^9` on NVIDIA GeForce RTX 2060 (with
1920 CUDA cores), demonstrates a rate of about 110M samples/sec on the GPU and a
latency of about 270 msec.
```
Facct sampling kernel took 74.587357 milliseconds
8388608 samples (0.962 success rate) took 341.727 milliseconds.
...
```

## Fast parallel matrix-multiplication over `Z_{2^w}`

Matrix-multiplication over `Z_{2^w}` is a bottleneck of the Lattice-ZK protocol
execution. For the protocol, matrix multiplication `X*Y=Z` is required only for
`X` in row-major order and `Y,Z` in column-major order, for both the CPU and the
GPU. A custom implementation using OpenMP for the CPU and CUDA for the GPU was
chosen after considering and rejecting BLAS, which was found to not support all
`w` in `{8,16,32}` and to output in double the (rather than the same) width of
the inputs. The matrix-multiplication CUDA implementation extends and enhances 
the matrix-vector-product implementation of the "Faster Matrix-Vector
Multiplication on GeForce 8800GTX" paper mainly as follows:
- Extension of the right-multiplicand from vector to column-major-order matrix.
- Enhancement to support integral-typed multiplication.
- Enhancement to 64-bit-wide texture-processing, not natively supported in CUDA.

## Acknowledgement

Work on the code in this repository was supported by ISF Grant No. 1399/17 and
Project PROMETHEUS (Grant 780701).
