#define MATLABCC_REPORT_STACK

#include <cmath>
#include <iostream>
#include <fstream>

#include <matlabcc>

void mexFunction(int nargout, mxArray *argout[], int nargin, const mxArray *argin[])
{
  mtb::Mat<double> imlab(argin[0]);
  mtb::Mat<double> kernel(argin[1]);
  
  argout[0] = mtb::create<3, double>(imlab.size(0), imlab.size(1), imlab.size(2));
  mtb::Mat<double, true> out(argout[0]);
  
  size_t kr = kernel.size(0) / 2;
  size_t kc = kernel.size(1) / 2;
  
  mtb::forEachElement<2>(imlab, [&](size_t row, size_t col) {
    double mean[3];
    double sum = mtb::meanKernel(mean, imlab, row, col, kernel, kr, kc);
    
    double a0 = 0, b0 = 0, c0 = 0;
    mtb::forKernel(imlab, row, col, kernel, kr, kc, [&](double kv, size_t row, size_t col) {
      a0 += kv*(imlab(row, col, 1) - mean[1]) * (imlab(row, col, 1) - mean[1]);
      b0 += kv*(imlab(row, col, 2) - mean[2]) * (imlab(row, col, 2) - mean[2]);
      c0 += kv*(imlab(row, col, 1) - mean[1]) * (imlab(row, col, 2) - mean[2]);
    });
    
    out(row, col, 0) = a0 / sum;
    out(row, col, 1) = b0 / sum;
    out(row, col, 2) = c0 / sum;
  });
}




