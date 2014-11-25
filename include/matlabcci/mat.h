#ifndef INCLUDE_MAT_H
#define INCLUDE_MAT_H

namespace mtb {

  
template <typename T, bool boundcheck = false>
class Mat {
  size_t *d_dims;
  size_t d_ndims;
  size_t d_numel;
  T *d_data;
  bool d_shared;
  T errorval;
  
public:
  Mat(mxArray const *arr)
  :
  d_dims(new size_t[mxGetNumberOfDimensions(arr)]),
  d_ndims(static_cast<size_t>(mxGetNumberOfDimensions(arr))),
  d_data((T *)mxGetData(arr)),
  d_shared(true)
  {
    init_dimensions(arr);
  }
  
  ~Mat()
  {
    delete[] d_dims;
    if (not d_shared)
      delete[] d_data;
  }
  
  inline size_t size(size_t dim = 0) const
  {
    return dim >= d_ndims ? 1 : d_dims[dim];
  }
  
  inline size_t dims() const { return d_ndims; }
  
  template<typename... Args>
  inline T &operator()(size_t first, Args... indices) { return normalat<0>(1, 0, first, indices...); }
  template<typename... Args>
  inline T &operator()(int    first, Args... indices) { return normalat<0>(1, 0, first, indices...); }
  
  template<typename... Args>
  inline T val(size_t  first, Args... indices) const { return normalat<0>(1, 0, first, indices...); }
  template<typename... Args>
  inline T val(int     first, Args... indices) const { return normalat<0>(1, 0, first, indices...); }
  
  template<typename... Args>
  inline T val(double first, Args... indices) const { return bilinearat<0>(1, 0, first, indices...); }
  template<typename... Args>
  inline T val(float  first, Args... indices) const { return bilinearat<0>(1, 0, first, indices...); }
  
  template<typename Lambda>
  friend inline void forEachLinear(Mat<T> matrix, Lambda f);
  
private:
  
  template<typename I>
  inline bool checkbound(I index, size_t dimension) {
    if (boundcheck && (index <0 || index >= size(dimension))) {
      err << "index " << index << " not in range [0.." << size(dimension) << ") for dimension " << dimension << ", for matrix at adress " << this << "\n" << ErrorType("matlabcc:IndexOutOfBound");
      return false;
    }
    return true;
  }
  
  template<size_t dimension>
  inline T &normalat(size_t factor, size_t so_far, size_t index)
  {
    if (not checkbound(index, dimension))
      return d_data[0];
    return d_data[so_far + factor * index];
  }
  
  template<size_t dimension, typename... Args>
  inline T &normalat(size_t factor, size_t so_far, size_t index, Args... indices)
  {
    if (not checkbound(index, dimension))
      return errorval;
    return normalat<dimension + 1>(size(dimension) * factor, so_far + factor * index, indices...);
  }
  
  template<size_t dimension>
  inline T normalat(size_t factor, size_t so_far, size_t index) const
  {
    if (not checkbound(index, dimension))
      return errorval;
    return d_data[so_far + factor * index];
  }
  
  template<size_t dimension, typename... Args>
  inline T normalat(size_t factor, size_t so_far, size_t index, Args... indices) const
  {
    if (not checkbound(index, dimension))
      return errorval;
    return normalat<dimension + 1>(size(dimension) * factor, so_far + factor * index, indices...);
  }
  
  template<size_t dimension>
  inline void bilinear(double v, size_t &low, size_t &high, double &f) const
  {
    low  = std::min(size(dimension) - 1, (size_t)(v < 0 ? 0 : std::floor(v)));
    high = std::min(size(dimension) - 1, (size_t)(v < 0 ? 0 : std::ceil (v)));
    f = std::fmod(v, 1.0);
    f = f < 0 ? 1 + f : f;
  }
  
  template<size_t dimension>
  inline T bilinearat(size_t factor, size_t so_far, double index) const
  {
    size_t low, high; double f;
    bilinear<dimension>(index, low, high, f);
    return (1 - f) * d_data[so_far + factor * low] +
                f  * d_data[so_far + factor * high];
  }
  
  template<size_t dimension, typename... Args>
  inline T bilinearat(size_t factor, size_t so_far, double index, Args... indices) const
  {
    size_t low, high; double f;
    bilinear<dimension>(index, low, high, f);
    return (1 - f) * bilinearat<dimension + 1>(size(dimension) * factor, so_far + factor * low, indices...) +
                f  * bilinearat<dimension + 1>(size(dimension) * factor, so_far + factor * high, indices...);
  }
  
  void init_dimensions(mxArray const *arr)
  {
    for(size_t i = 0; i < d_ndims; ++i)
    {
      d_dims[i] = static_cast<size_t>(mxGetDimensions(arr)[i]);
      d_numel *= d_dims[i];
    }
  }
};
}

#include "create.h"

#include "foreach.h"

#endif
