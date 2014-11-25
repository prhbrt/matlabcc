#ifndef MATLABCC_CREATE_H
#define MATLABCC_CREATE_H

namespace mtb {

  
template<typename T>
mxClassID type() {
  err << "Unknown mex type\n" << ErrorType("matlabcc:UnknownType");
  return mxUINT8_CLASS;
}

template<>
mxClassID type<uint8_t>() { return mxUINT8_CLASS; }
template<>
mxClassID type<int8_t>() { return mxINT8_CLASS; }
template<>
mxClassID type<uint16_t>() { return mxUINT16_CLASS; }
template<>
mxClassID type<int16_t>() { return mxINT16_CLASS; }
template<>
mxClassID type<uint32_t>() { return mxUINT32_CLASS; }
template<>
mxClassID type<int32_t>() { return mxINT32_CLASS; } 
template<>
mxClassID type<uint64_t>() { return mxUINT64_CLASS; }
template<>
mxClassID type<int64_t>() { return mxINT64_CLASS; } 
template<>
mxClassID type<float>() { return mxSINGLE_CLASS; }
template<>
mxClassID type<double>() { return mxDOUBLE_CLASS; } 


template<bool c>
mxComplexity complex() { throw 0; }
template<>
mxComplexity complex<false>() { return mxREAL; }
template<>
mxComplexity complex<true>() { return mxCOMPLEX; } 

template<size_t dimensions, typename T = double, bool c = false>
struct _create {
  mwSize dims[dimensions];
  mxArray *run(size_t dim) {
    return mxCreateNumericArray(dimensions, dims, type<T>(), complex<c>());
  }
  
  template <typename... Args>
  mxArray *run(size_t dim, size_t index, Args... sizes) {
    dims[dim] = static_cast<mwSize>(index);
    return run(dim + 1, sizes...);
  }
};

template <size_t dimensions, typename T = double, bool cmplx = false, typename... Args>
mxArray *create(Args... sizes){
  _create<dimensions, T, cmplx> cr;
  mxArray *r = cr.run(0, sizes...);
  return r;
}


}
#endif
