#ifndef MATLABCC_ERROR_H
#define MATLABCC_ERROR_H


#include <string>
#include <iostream>
#include <stdio.h>


namespace mtb {
  
class ErrorType {
  char const *d_type;
public:
  ErrorType(char const *type) : d_type(type) {}
  char const *type() { return d_type; }
};

class ErrorReporter {
  std::stringstream ss;
public:
  template<typename T>
  friend ErrorReporter &operator<<(ErrorReporter &, T);
  void error(char const *type) {
#ifdef MATLABCC_REPORT_STACK
    std::cerr << stck::stacktrace;
#endif
    char const *m = ss.str().c_str();
    ss.str("");
    ss.clear();
    mexErrMsgIdAndTxt(type, m);
  }
};


template<typename T>
ErrorReporter &operator<<(ErrorReporter &err, T val) {
  err.ss << val;
  return err;
}

template<>
ErrorReporter &operator<< <ErrorType>(ErrorReporter &err, ErrorType val) {
  err.error(val.type());
  return err;
}

ErrorReporter err;


}

#endif