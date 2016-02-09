## STATUS

EXPERIMENTAL: If you use this, be prepared to be dissapointed.

## INSTALL

Just run install to install the header file in /usr/include

## USAGE

include the header file like this:

    #include <matlabcc.h>

That's all, everything is in this header file.

## COMPILE:

Comple your mex-files like so:

    mex CXXFLAGS="-Wall -std=c++11 -fPIC -O2" file.cpp

Make sure the extension is .cpp, otherwise mex uses gcc instead of g++ (I think).
-O2 is particularly important for optimization of using lambda-functions. They are,
to the best of my knowledge, not inlined without -O2. Also see my question:

http://stackoverflow.com/questions/27058847/fast-and-generic-use-of-lambda-functions

Warning: I am not an expert.

## EXAMPLE:


``` c++11
    //Consider this to be the mexFunction definition:
    void mexFunction(int nargout, mxArray *argout[], int nargin, const mxArray *argin[]) ...


    //Read several dimensions (as many as you want) of a matrix mxArray. Superfluos dimensions are set to 1.
    size_t height, width, channels;
    dimensions(argin[0], height, width, channels);

    //Get a specific element for a MxNxC or MxN matrix:

    double *data = data<double>(argin[0]);
    dimensions(argin[0], height, width, channels);

    double val = at(data, height, width, row, col, channel);

    at(data, height, width, row, col, channel) = 5;

    // There is also a Mat<T> class. It has nice functionality, but is still very experimental.
    // One cool, but possibly slow, way to use this class is like this:

    #include <cmath>
    #include <matrix.h>
    #include <mex.h>

    // #define MATLIB_BOUNDCHECK

    #include <matlabcc.h>

    typedef mtb::Mat<double> Image;
    typedef mtb::Mat<int> Indices;

    mxArray *initOutput(Image &im, mwSize channels) {
    mwSize const dims[3] = {static_cast<int>(im.size(0)), static_cast<int>(im.size(1)), channels};
    return mxCreateNumericArray(3, dims, mxDOUBLE_CLASS, mxREAL);
    }

    void mexFunction(int nargout, mxArray *argout[], int nargin, const mxArray *argin[]) {
    Image im(argin[0]); // std::cout << "im: " << &im << '\n';
    Image X(argin[1]);  // std::cout << " X: " << &X << '\n';
    Image Y(argin[2]);  // std::cout << " Y: " << &Y << '\n';
    
    argout[0] = initOutput(X, im.size(2));
    Image out(argout[0]); //std::cout << "ou: " << &out << '\n';
    
    X.forEach2([&im, &out, &Y](Image &X, size_t row, size_t col) {
        for(size_t c = 0; c < im.size(2); ++c)
        out(row, col, c) = bilinear(im, X(row, col), Y(row, col), c);
    });
    }



    // Also check out forKernel, this allows you to easily apply a sliding window on a MxN or MxNxC matrix,
    // in a very generic way.

    #include <cmath>
    #include <matrix.h>
    #include <mex.h>

    #include "matlabpp.h"

    typedef mtb::Mat<double> Image;

    mxArray *initOutput(Image &im) {
    mwSize const dims[3] = {static_cast<int>(im.size(0)), static_cast<int>(im.size(1)), static_cast<int>(im.size(2))};  
    return mxCreateNumericArray(3, dims, mxDOUBLE_CLASS, mxREAL);
    }

    void mexFunction(int nargout, mxArray *argout[], int nargin, const mxArray *argin[]) {
    Image im0(argin[0]);
    Image im1(argin[1]);
    Image kernel(argin[2]);
    
    double kr = mtb::val<double>(argin[3], 0);
    double kc = mtb::val<double>(argin[3], 1);
    
    argout[0] = initOutput(im0);
    Image ssimmap(argout[0]);
    
    for (size_t channel = 0; channel < im0.size(2); ++channel) {
        im0.forEach([...](Image &im0, size_t row, size_t col) {
        
        im0.forKernel<double>(row, col, kernel, kr, kc, [...](Image &im0, double w, size_t row, size_t col) {
            // ... 
        });
        });
    }
    }
```
