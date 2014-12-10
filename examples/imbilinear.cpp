#define MATLABCC_REPORT_STACK
#include <matlabcc>

void mexFunction(int nargout, mxArray *argout[], int nargin, const mxArray *argin[])
{
  if (not mtb::isa<double>(argin[0])) {
    mtb::err << "First argument should be double\n" << mtb::ErrorType("matlabcc:IlligalArgument"); return; }
  
  if (not mtb::isa<double>(argin[1])) {
    mtb::err << "Second argument should be double\n" << mtb::ErrorType("matlabcc:IlligalArgument"); return; }
  
  if (not mtb::isa<double>(argin[2])) {
    mtb::err << "Third argument should be double\n" << mtb::ErrorType("matlabcc:IlligalArgument"); return; }
  
  if (not mtb::same_size(argin[1], argin[2])) {
    mtb::err << "Second and third argument should have the same size\n" << mtb::ErrorType("matlabcc:IlligalArgument"); return; }
  
  if(nargout != 1) {
    mtb::err << "Exactly one output argument is needed\n" << mtb::ErrorType("matlabcc:IlligalArgument"); return; }
  
  mtb::Mat<double> matrix(argin[0]);
  mtb::Mat<double> rowindices(argin[1]);
  mtb::Mat<double> colindices(argin[2]);
  
  argout[0] = mtb::create<3, double, false>(rowindices.size(0), rowindices.size(1), matrix.size(2));
  mtb::Mat<double> out(argout[0]);
  
  mtb::forEachElement<2> (rowindices, [&](size_t row, size_t col) {
    for (size_t channel = 0; channel < matrix.size(2); ++channel)
      out(row, col, channel) = matrix.val(rowindices(row, col), colindices(row, col), channel);
  });
}


