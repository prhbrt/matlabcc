#define MATLABCC_REPORT_STACK

#include <cmath>
#include <iostream>
#include <fstream>

#include <matlabcc>
#include <iostream>
#include <stdio.h>

static void HandleError(
  cudaError_t err,
  const char *file,
  int line )
{
  if (err != cudaSuccess)
  {
    printf( "%s in %s at line %d\n", cudaGetErrorString( err ), file, line );
    exit( EXIT_FAILURE );
  }
}
#define HANDLE_ERROR( err ) (HandleError( err, __FILE__, __LINE__ ))


#define HANDLE_NULL( a ) \\
{ \\
  if (a == NULL)\\
  { \\
    printf( "Host memory failed in %s at line %d\n", __FILE__, __LINE__ ); \\
    exit( EXIT_FAILURE ); \\
  } \\
}


#define N 1400
#define K 15
#define K2 ((K - 1) / 2)


template<int mx, int my>
__device__ inline int index(int x, int y)
{
  return x*my + y;
}

__global__ void ssimkernel(double *test, double *reference, double *kernel, double *ssim)
{
  int x = blockIdx.x;
  int y = blockIdx.y;
  
  double mean_test = 0;
  double mean_reference = 0;
  for(int i=0; i<K; ++i)
  for(int j=0; j<K; ++j)
  {
    if (x + i - K2 >= 0 and
        x + i - K2 < N  and
        y + j - K2 >= 0 and
        y + j - K2 < N)
    {
      mean_test      +=  kernel[index<K,K>(i,j)] * test[index<N,N>(x+i-K2, y+j-K2)];
      mean_reference +=  kernel[index<K,K>(i,j)] * reference[index<N,N>(x+i-K2, y+j-K2)];
    }
  }
  
  double var_test = 0;
  double var_reference = 0;
  double correlation = 0;
  
  for(int i=0; i<K; ++i)
  for(int j=0; j<K; ++j)
  {
    if (x + i - K2 >= 0 and
        x + i - K2 < N  and
        y + j - K2 >= 0 and
        y + j - K2 < N)
    {
      double a = (test[index<N,N>(x+i-K2, y+j-K2)] - mean_test);
      double b = (reference[index<N,N>(x+i-K2, y+j-K2)] - mean_reference);
      var_test      += kernel[index<K,K>(i,j)] * a * a;
      var_reference += kernel[index<K,K>(i,j)] * b * b;
      correlation   += kernel[index<K,K>(i,j)] * a * b;
    }
  }
}


template<typename T>
inline T sim(T v0, T v1, T c)
{
  return (c + 2*v0*v1) / (c + v1*v1 + v0*v0);
}

void mexFunction(int nargout, mxArray *argout[], int nargin, const mxArray *argin[])
{
  mtb::Mat<double> im ();
  mtb::Mat<double> imr(argin[1]);
  mtb::Mat<double> kernel(argin[2]);

  double *test      = (double *)mxGetData(argin[0]);
  double *reference = (double *)mxGetData(argin[1]);
  double *kernel    = (double *)mxGetData(argin[2]);
  
  mxSize rows = 
  mxSize cols = 
  
  
  double *kernel    = (double *)mxGetData(argin[2]);
  double *ssim      = new double[N * N];
  
  double *test_cuda;
  double *reference_cuda;
  double *kernel_cuda;
  double *ssim_cuda;
  HANDLE_ERROR( cudaMalloc((void **) &test_cuda,      N*N*sizeof(double)) );
  HANDLE_ERROR( cudaMalloc((void **) &reference_cuda, N*N*sizeof(double)) );
  HANDLE_ERROR( cudaMalloc((void **) &kernel_cuda,    K*K*sizeof(double)) );
  HANDLE_ERROR( cudaMalloc((void **) &ssim_cuda,      N*N*sizeof(double)) );
  
  HANDLE_ERROR( cudaMemcpy(test_cuda,      test,      N*N*sizeof(double), cudaMemcpyHostToDevice) );
  HANDLE_ERROR( cudaMemcpy(reference_cuda, reference, N*N*sizeof(double), cudaMemcpyHostToDevice) );
  HANDLE_ERROR( cudaMemcpy(kernel_cuda,    kernel,    K*K*sizeof(double), cudaMemcpyHostToDevice) );
  
  dim3 grid   ( N, N );
  ssimkernel<<<grid, 1>>>(test_cuda, reference_cuda, kernel_cuda, ssim_cuda);
  
  cudaFree( test_cuda );
  cudaFree( reference_cuda );
  cudaFree( kernel_cuda );
  cudaFree( ssim_cuda );
  
  delete[] test;
  delete[] reference;
  delete[] kernel;
  delete[] ssim;
  
  argout[0] = mtb::create<3, double>(im.size(0), im.size(1), im.size(2));
  mtb::Mat<double, false> ssim(argout[0]);
  
  size_t kr = kernel.size(0) / 2;
  size_t kc = kernel.size(1) / 2;
  
  mtb::forEachElement<3>(im, [&](size_t row, size_t col, size_t channel) {
    double mean = 0, meanr = 0;
    double sum = 0;;
    mtb::forKernel(im, row, col, kernel, kr, kc, [&](double kv, size_t row, size_t col) {
      mean  += kv * im (row, col, channel);
      meanr += kv * imr(row, col, channel);
      sum += kv;
    });
    mean  /= sum;
    meanr /= sum;
    
    double var = 0, varr = 0, cov = 0;
    mtb::forKernel(im, row, col, kernel, kr, kc, [&](double kv, size_t row, size_t col) {
      var  += kv * (im (row, col, channel) - mean ) * (im (row, col, channel) - mean );
      varr += kv * (imr(row, col, channel) - meanr) * (imr(row, col, channel) - meanr);
      cov  += kv * (im (row, col, channel) - mean ) * (imr(row, col, channel) - meanr);
    });
    var  /= sum;
    varr /= sum;
    cov  /= sum;
    //var = std::sqrt(var);
    //varr = std::sqrt(varr);
    
    ssim(row, col, channel) = sim(mean, meanr, 0.01) * (0.03 + 2*cov) / (0.03 + var + varr);
  });
}
