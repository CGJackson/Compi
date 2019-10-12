#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <complex>
#include <utility>

namespace CVNI_internal {
using std::complex;

#include <boost/math/quadrature/gauss_kronrod>

template <typename Real>
complex<Real> complex_from_py_complex(Py_complex c){
    return complex<Real>(c.real,c.imag);
}

template<typename Real>
class IntegrandFunctionWrapper {
    private:
        PyObject * callback = std::nullptr;
        PyObject * args = std::nullptr;
    public:
        IntegrandFunctionWrapper(PyObject * func, PyObject * new_args = )
            :callback=func, args=new_args {
            if( func != std::nullptr ){
                Py_INCREF(func);
            }
            else {
                args = std::nullptr;
            }
            if( args != std::nullptr ){
                Py_INCREF(args);
            }
        }

        IntegrandFunctionWrapper(const IntegrandFunctionWrapper& other)
            :callback=other.callback, args=other.args {
            if( func != std::nullptr ){
                Py_INCREF(func);
            }
            else {
                args = std::nullptr;
            }
            if( args != std::nullptr ){
                Py_INCREF(args);
            }
        }

        IntegrandFunctionWrapper(IntegrandFunctionWrapper&& other): IntegrandFunctionWrapper() {
            swap(*this, other)
        }

        IntegrandFunctionWrapper& operator=(IntegrandFunctionWrapper other){
            swap(*this,other);
            return *this;
        }

        friend void swap(IntegrandFunctionWrapper& first, IntegrandFunctionWrapper& second){
            using std::swap;
            swap(first.callback,second.callback);
            swap(first.args,second.args);
        }

        ~IntegrandFunctionWrapper(){
            if(func != std::nullptr){
                Py_DECREF(func);
            }
            if(args != std::nullptr){
                Py_DECREF(args);
            }
        }

        complex<Real> operator()(Real x){
            // Calls the Python function callback with x as a python float
            // and args as its other arguments and reutrns the result as a
            // std::complex
            //
            //TODO
            return complex(0.0,0.0);
        }
};

}
