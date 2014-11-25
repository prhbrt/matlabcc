#ifndef MATLABCC_FOREACH_H
#define MATLABCC_FOREACH_H

namespace mtb {


template<typename T, typename Lambda>
inline void forEachLinear(Mat<T> matrix, Lambda const &f) {
  size_t n = matrix.numel();
  for (size_t idx = 0; idx < n; ++idx)
    f(matrix.d_data[idx]);
}

template<size_t dimension>
struct _forEachElement {
  template<typename T, typename L, typename... Args>
  inline void run(Mat<T> &matrix, L const &f, Args... args) {
    size_t sz = matrix.size(dimension - 1);
    _forEachElement<dimension - 1> fe;
    for (size_t i = 0; i < sz; ++i)
      fe.run(matrix, f, i, args...);
  }
};

template<>
struct _forEachElement<0> {
  template<typename T,typename L,typename... Args>
  inline void run(Mat<T> &matrix, L const &f, Args... args) {
    f(args...);
  }
};

template<>
struct _forEachElement<1> {
  template<typename T,typename L,typename... Args>
  inline void run(Mat<T> &matrix, L const &f, Args... args) {
    size_t sz0 = matrix.size(0);
    for (size_t i0 = 0; i0 < sz0; ++i0)
      f(i0, args...);
  }
};

template<>
struct _forEachElement<2> {
  template<typename T,typename L,typename... Args>
  inline void run(Mat<T> &matrix, L const &f, Args... args) {
    size_t sz0 = matrix.size(0);
    size_t sz1 = matrix.size(1);
    for (size_t i1 = 0; i1 < sz1; ++i1)
    for (size_t i0 = 0; i0 < sz0; ++i0)
      f(i0, i1, args...);
  }
};

template<>
struct _forEachElement<3> {
  template<typename T,typename L,typename... Args>
  inline void run(Mat<T> &matrix, L const &f, Args... args) {
    size_t sz0 = matrix.size(0);
    size_t sz1 = matrix.size(1);
    size_t sz2 = matrix.size(2);
    for (size_t i2 = 0; i2 < sz2; ++i2)
    for (size_t i1 = 0; i1 < sz1; ++i1)
    for (size_t i0 = 0; i0 < sz0; ++i0)
      f(i0, i1, i2, args...);
  }
};

template<>
struct _forEachElement<4> {
  template<typename T,typename L,typename... Args>
  inline void run(Mat<T> &matrix, L const &f, Args... args) {
    size_t sz0 = matrix.size(0);
    size_t sz1 = matrix.size(1);
    size_t sz2 = matrix.size(2);
    size_t sz3 = matrix.size(3);
    for (size_t i3 = 0; i3 < sz3; ++i3)
    for (size_t i2 = 0; i2 < sz2; ++i2)
    for (size_t i1 = 0; i1 < sz1; ++i1)
    for (size_t i0 = 0; i0 < sz0; ++i0)
      f(i0, i1, i2, i3, args...);
  }
};

template<>
struct _forEachElement<5> {
  template<typename T,typename L,typename... Args>
  inline void run(Mat<T> &matrix, L const &f, Args... args) {
    size_t sz0 = matrix.size(0);
    size_t sz1 = matrix.size(1);
    size_t sz2 = matrix.size(2);
    size_t sz3 = matrix.size(3);
    size_t sz4 = matrix.size(4);
    for (size_t i4 = 0; i4 < sz4; ++i4)
    for (size_t i3 = 0; i3 < sz3; ++i3)
    for (size_t i2 = 0; i2 < sz2; ++i2)
    for (size_t i1 = 0; i1 < sz1; ++i1)
    for (size_t i0 = 0; i0 < sz0; ++i0)
      f(i0, i1, i2, i3, i4, args...);
  }
};

template<size_t dimensions, typename T, typename L>
inline void forEachElement(Mat<T> &matrix, L const &f) {
  _forEachElement<dimensions> fe;
  fe.run(matrix, f);
}


template<typename T, typename K, typename Lambda>
inline void forKernel(Mat<T> &matrix, size_t index, Mat<K> &kernel, size_t center, Lambda const &f) {
  size_t ksz0 = kernel.size(0);
  size_t sz0  = matrix.size(0);
  size_t max  = ksz0 + 1 + index < sz0 + center ? ksz0 : center + sz0 - 1 - index;
  
  for (size_t i = (center > index ? center - index : 0); i < max; ++i)
    f(kernel(i), index - center + i);
}

template<typename T, typename K, typename Lambda>
inline void forKernel(Mat<T> &matrix, size_t row, size_t col, Mat<K> &kernel, size_t center_row, size_t center_col, Lambda const &f) {
  size_t ksz0 = kernel.size(0);
  size_t sz0  = matrix.size(0);
  size_t max_row = ksz0 + 1 + row < sz0 + center_row ? ksz0 : center_row + sz0 - 1 - row;
  
  size_t ksz1 = kernel.size(1);
  size_t sz1  = matrix.size(1);
  size_t max_col = ksz1 + 1 + col < sz1 + center_col ? ksz1 : center_col + sz1 - 1 - col;
  
  for (size_t c = (center_col > col ? center_col - col : 0); c < max_col; ++c)
  for (size_t r = (center_row > row ? center_row - row : 0); r < max_row; ++r)
    f(kernel(r, c), row - center_row + r, col - center_col + c);
}


}
#endif
