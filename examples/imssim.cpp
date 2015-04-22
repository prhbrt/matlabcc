#define MATLABCC_REPORT_STACK

#include <cmath>
#include <iostream>
#include <fstream>

#include <matlabcc>


template<typename T>
inline T sim(T v0, T v1, T c)
{
  return (c + 2*v0*v1) / (c + v1*v1 + v0*v0);
}

void mexFunction(int nargout, mxArray *argout[], int nargin, const mxArray *argin[])
{
  mtb::Mat<double> im (argin[0]);
  mtb::Mat<double> imr(argin[1]);
  mtb::Mat<double> kernel(argin[2]);
  
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
