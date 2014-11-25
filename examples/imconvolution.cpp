#define MATLABCC_REPORT_STACK
#include <matlabcc>

void mexFunction(int nargout, mxArray *argout[], int nargin, const mxArray *argin[])
{
  mtb::Mat<double> matrix(argin[0]);
  mtb::Mat<double> kernel(argin[1]);
  argout[0] = mtb::create<2, bool>(matrix.size(0), matrix.size(1));
  mtb::Mat<double, true> out(argout[0]);
  
  size_t kr = kernel.size(0) / 2;
  size_t kc = kernel.size(1) / 2;
  
  mtb::forEachElement<2>(matrix, [&out, &matrix, &kernel, kr, kc](size_t row, size_t col) {
    double v = 0;
    mtb::forKernel(matrix, row, col, kernel, kr, kc, [&v, &matrix](double kv, size_t row, size_t col) {
      v+= kv*matrix(row, col);
    });
    out(row, col) = v;
  });
}
