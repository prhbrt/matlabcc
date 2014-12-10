#define MATLABCC_REPORT_STACK

#include <cmath>
#include <iostream>
#include <fstream>

#include <matlabcc>

inline double sq(double x) { return x*x; }
inline double sqlog2(double x) { return sq(std::log2(x)); }

size_t abcsolve(double &x0, double &x1, double a, double b, double c) {
  double d = b*b - 4*a*c;
  if (d < 0) return 0;
  if (d == 0) {
    x0 = -b / (2*a);
    return 1;
  }
  d = std::sqrt(d);
  x0 = (-b - d) / (2*a);
  x1 = (-b + d) / (2*a);
  return 2;
}

void mexFunction(int nargout, mxArray *argout[], int nargin, const mxArray *argin[])
{
  
  mtb::Mat<double> im0(argin[0]);
  mtb::Mat<double> im1(argin[1]);
  
  mtb::Mat<double> kernel(argin[2]);
  
  argout[0] = mtb::create<3, double>(im0.size(0), im1.size(1), im1.size(2));
  mtb::Mat<double, true> out(argout[0]);
  
  size_t kr = kernel.size(0) / 2;
  size_t kc = kernel.size(1) / 2;
  
  std::ofstream log("covariances.txt");
  mtb::forEachElement<2>(im0, [&](size_t row, size_t col) {
    double m0[3] = {0, 0, 0};
    double m1[3] = {0, 0, 0};
    
    size_t n = kernel.size(0) * kernel.size(1);
    mtb::forKernel(im0, row, col, kernel, kr, kc, [&](double kv, size_t row, size_t col) {
      mtb::staticfor<3>([&](size_t channel) {
        m0[channel] += im0(row, col, channel);
        m1[channel] += im1(row, col, channel);
      });
    });
    
    mtb::staticfor<3>([&](size_t channel) {
      m0[channel] /= n;
      m1[channel] /= n;
    });
    
    double a0 = 0, b0 = 0, c0 = 0;
    
    mtb::forKernel(im0, row, col, kernel, kr, kc, [&](double kv, size_t row, size_t col) {
      a0 += (im0(row, col, 1) - m0[1]) * (im0(row, col, 1) - m0[1]);
      b0 += (im0(row, col, 2) - m0[2]) * (im0(row, col, 2) - m0[2]);
      c0 += (im0(row, col, 1) - m0[1]) * (im0(row, col, 2) - m0[2]);
      
      //a1 += (im1(row, col, 1) - m1[1]) * (im1(row, col, 1) - m1[1]);
      //b1 += (im1(row, col, 2) - m1[2]) * (im1(row, col, 2) - m1[2]);
      //c1 += (im1(row, col, 1) - m1[1]) * (im1(row, col, 2) - m1[2]);
    });
    
    //double a = a0*b0 + c0*c0;
    //double b = 2*c0*c1 - a0*b1 - b0*a1;
    //double c = a1*b1 + c1*c1;
    
    //double x0 = 0, x1 = 0;
    //size_t nn = abcsolve(x0, x1, a, b, c);
    //log << '[' << a0 << ' ' << b0 << ' ' << c0 << "] [" << a1 << ' ' << b1 << ' ' << c1 << "]\n";
    out(row, col, 0) = a0;
    out(row, col, 1) = b0;
    out(row, col, 2) = c0;
  });
  log.close();
}




