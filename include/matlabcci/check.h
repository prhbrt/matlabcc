#ifndef MATLABCC_CHECK_H
#define MATLABCC_CHECK_H

namespace mtb {


template<typename T>
inline bool isa(mxArray const *arr) { return type<T>() == mxGetClassID(arr); }

bool same_size(mxArray const *arr0, mxArray const *arr1) {
  size_t nd0 = static_cast<size_t>(mxGetNumberOfDimensions(arr0));
  size_t nd1 = static_cast<size_t>(mxGetNumberOfDimensions(arr1));
  const mwSize *sz0 = mxGetDimensions(arr0);
  const mwSize *sz1 = mxGetDimensions(arr1);
  size_t i = 0;
  for (; i < nd0 and i < nd1; ++i)
    if (sz0[i] != sz1[i])
      return false;
  
  for (; i < nd0; ++i) if (sz0[i] != 1) return false;
  for (; i < nd1; ++i) if (sz1[i] != 1) return false;
  return true;
}


}
#endif
