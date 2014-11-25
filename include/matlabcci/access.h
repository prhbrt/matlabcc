#ifndef MATLABCC_ACCESS_H
#define MATLABCC_ACCESS_H


namespace mtb {

size_t dims(mxArray const *arr) {
  return static_cast<size_t>(mxGetNumberOfDimensions(arr));
}

template<typename T>
inline T &at(T *arr, size_t height, size_t row, size_t col) {
  return arr[row + height*col];
}

template<typename T>
inline T &at(T *arr, size_t height, size_t width, size_t row, size_t col, size_t channel) {
  return arr[row + height*(col + width*channel)];
}

template<typename T>
T const val(mxArray const *arr, size_t idx=0) {
  return reinterpret_cast<T *>(mxGetData(arr))[idx];
}

template<typename T>
T const *data(mxArray const *arr) {
  return reinterpret_cast<T const *>(mxGetData(arr));
}

template<typename T>
T *data(mxArray *arr) {
  return reinterpret_cast<T *>(mxGetData(arr));
}

template<typename T>
T &val(mxArray *arr, size_t idx=0) {
  return reinterpret_cast<T *>(mxGetData(arr))[idx];
}

void _dimensions (mwSize ndims, mwSize const *adims, size_t &dim0)
{
  dim0 = ndims <= 0 ? 1 : static_cast<size_t>(*adims);
}

template<typename... Args>
void _dimensions(mwSize ndims, mwSize const *adims, size_t &dim0, Args &... dims)
{
  dim0 = ndims <= 0 ? 1 : static_cast<size_t>(*adims);
  _dimensions(ndims - 1, adims + 1, dims...);
}

template<typename... Args>
void dimensions(mxArray const *arr, Args &... dims)
{
  mwSize ndims = mxGetNumberOfDimensions(arr);
  mwSize const *adims = mxGetDimensions(arr);
  _dimensions(ndims, adims, dims...);
}


}

#endif