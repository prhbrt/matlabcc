// author: Herbert Kruitbosch, CC: be nice.
#ifndef MATLABPP_H
#define MATLABPP_H

#include <matrix.h>
#include <mex.h>

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <functional>
#include <cmath>


namespace mtb {
  size_t dims(mxArray const *arr) {
    return static_cast<size_t>(mxGetNumberOfDimensions(arr));
  }
  
  bool same_size(mxArray const *arr0, mxArray const *arr1) {
    mwSize nd0 = mxGetNumberOfDimensions(arr0);
    mwSize nd1 = mxGetNumberOfDimensions(arr0);
    mwSize const *d0 = mxGetDimensions(arr0);
    mwSize const *d1 = mxGetDimensions(arr1);
    
    for (int i = 0; i < nd0 || i < nd1; ++i)
      if ( (i < nd0 ? d0[i] : 1) != (i < nd1 ? d1[i] : 1) )
        return false;
    return true;
  }
  
  template<typename T>
  inline T square(T x) { return x*x; }
  
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

  template <typename T>
  class Mat {
  public:
    Mat(mxArray const *arr)
    : d_dims(new size_t[mxGetNumberOfDimensions(arr)]),
      d_ndims(static_cast<size_t>(mxGetNumberOfDimensions(arr))),
      d_data((T *)mxGetData(arr)),
      d_shared(true)
    {
      d_numel = (d_ndims > 0 ? 1 : 0);
      for(size_t i = 0; i < d_ndims; ++i) {
        d_dims[i] = mxGetDimensions(arr)[i];
        d_numel *= d_dims[i];
      }
    }
    
    Mat(std::initializer_list<size_t> dims)
    : d_dims(new size_t[dims.size()]),
      d_ndims(dims.size()),
      d_shared(false)
    {
      d_numel = (dims.size() > 0 ? 1 : 0);
      size_t *d = d_dims;
      for(auto it = dims.begin(); it != dims.end(); ++it, ++d) {
        *d = *it;
        d_numel *= *d;
      }
      d_data = new T[d_numel];
    }
    
    ~Mat() {
      delete[] d_dims;
      if (not d_shared) {
        delete[] d_data;
      }
    }
    
    template<typename... Args>
    inline T operator()(Args... idxs) const
    {
      return d_data[rawidx(0, 1, idxs...)];
    }
    
    template<typename... Args>
    inline T &operator()(Args... idxs)
    {
      return d_data[rawidx(0, 1, idxs...)];
    }
    
    inline size_t size(size_t dim = 0) const { return dim >= d_ndims ? 1 : static_cast<size_t>(d_dims[dim]); }
    inline size_t numdims() const { return static_cast<size_t>(d_ndims); }
    inline size_t numel() const {
      return d_numel;
    }
    
    Mat<T> &operator=(T val) {
      forEach([val](T &elem) {
        elem = val;
      });
      return *this;
    }
    
    template<typename S=T, size_t w=3>
    void print()
    {
      bool newstart = true;
      size_t bfact = d_ndims > 2 ? d_dims[0]*d_dims[1] : 1;
      for (size_t i = 0; i < numel(); ++i)
      {
        if (d_ndims > 2 and newstart)
        {
          std::cout << "\nvalues for (:,:";
          size_t fact = bfact;
          for (size_t j = 2; j < d_ndims; ++j)
          {
            size_t idx = (i / fact) % size(j);
            std::cout << "," << idx;
            fact *= d_dims[j];
          }
          std::cout << "):\n";
          newstart = false;
        }
        std::cout << std::setw(w) << static_cast<S>(d_data[i]) << " ";
        if ((i + 1) % d_dims[0] == 0)
        {
          newstart = (i + 1) % bfact == 0;
          std::cout << '\n';
        }
      }    
    }
    
    template<typename Lambda>
    inline void forEachn(Lambda f) {
      for (size_t idx = 0; idx < numel(); ++idx)
        f(d_data[idx]);
    }
    
    template<typename Lambda>
    inline void forEach2(Lambda f) {
      for (size_t row = 0; row < size(0); ++row)
        for (size_t col = 0; col < size(1); ++col) {
          f(*this, row, col);
        }
    }
    
    template<typename Lambda>
    inline void forEach1(Lambda f) {
      for (size_t row = 0; row < size(0); ++row)
          f(*this, row);
    }
    
    template<typename Lambda>
    inline void forEach1(Lambda f) const {
      for (size_t row = 0; row < size(0); ++row)
          f(*this, row);
    }
    
    template<typename K, typename Lambda>
    inline void forKernel(size_t row, size_t col, Mat<K> &kernel, size_t krow, size_t kcol, Lambda f) {
      for (int r = std::max(0, (int)krow - (int)row); r < std::min((int)kernel.size(0), (int)krow + (int)size(0) - 1 - (int)row); ++r)
        for (int c = std::max(0, (int)kcol - (int)col); c < std::min((int)kernel.size(1), (int)kcol + (int)size(1) - 1 - (int)col); ++c)
          f(*this, kernel(r, c), row - krow + r, col - kcol + c);
    }
    
    template<typename Lambda>
    inline size_t forWindow(size_t row, size_t col, size_t radius, Lambda f) {
      size_t pixels = 0;
      for (int r = -1*std::min(static_cast<int>(radius), static_cast<int>(row)); r <= std::min(static_cast<int>(radius), static_cast<int>(size(0) - row) - 1); ++r)
      {
        for (int c = -1*std::min(static_cast<int>(radius), static_cast<int>(col)); c <= std::min(static_cast<int>(radius), static_cast<int>(size(1) - col) - 1); ++c)
        {
          f(*this, row+r, col+c);
          ++pixels;
        }
      }
      return pixels;
    }
    
  private:
    inline size_t rawidx(size_t dim, size_t fact, size_t idx) const {
#ifdef MATLIB_BOUNDCHECK
      size_t sz = size(dim);
      if (idx <0 || idx >= sz)
      {
        std::cerr << "index " << idx << " not in range [0.." << sz << ") for dimension " << dim << ", for image at adress " << this << "\n";
        mexErrMsgIdAndTxt("IndexOutOfBound", "Index out of bounds");
        return 0;
      }
#endif
      return fact * idx;
    }
    template<typename... Args>
    inline size_t rawidx(size_t dim, size_t fact, size_t idx, Args... args) const {
    size_t sz = size(dim);
#ifdef MATLIB_BOUNDCHECK
      if (idx <0 || idx >= sz)
      {
        std::cerr << "index " << idx << " out of bound for dimension " << dim << " of size [0.." << sz << ")" << ", for image at adress " << this << "\n";
        mexErrMsgIdAndTxt("IndexOutOfBound", "Index out of bounds");
        return 0;
      }
#endif
      return fact * idx + rawidx(dim + 1, sz * fact, args...);
    }
    
    size_t *d_dims;
    size_t d_ndims;
    size_t d_numel;
    T *d_data;
    bool d_shared;
  };

  template<typename T>
  T dot(Mat<T> const &h0, Mat<T> const &h1) {
    T r = 0;
    h0.forEach([&r, &h1](Mat<T> const &h0, size_t i) {
      r += h0(i) * h1(i);
    });
    return r;
  }

  template<typename T>
  T bhattacharya(Mat<T> const &h0, Mat<T> const &h1) {
    T r = 0;
    h0.forEach([&r, &h1](Mat<T> const &h0, size_t i) {
      r += std::sqrt(h0(i) * h1(i));
    });
    return r;
  }
  
  void _bilinear(double v, int &low, int &high, double &f, int sz)
  {
    low  = std::max(0, std::min(sz - 1, (int)std::floor(v)));
    high = std::max(0, std::min(sz - 1, (int)std::ceil (v)));
    f = std::fmod(v, 1.0);
    f = f < 0 ? 1 + f : f;
  }
  
  double bilinear(Mat<double> &im, double row, double col, int c) {
    int rowl, rowh, coll, colh;
    double colf, rowf;
    _bilinear(row, rowl, rowh, rowf, im.size(0));
    _bilinear(col, coll, colh, colf, im.size(1));
    return (1 - rowf) * ((1 - colf) * im(rowl, coll, c) + colf * im(rowl, colh, c)) +
                rowf  * ((1 - colf) * im(rowh, coll, c) + colf * im(rowh, colh, c));
  }

}
#endif