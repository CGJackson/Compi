#include "ComplexValuedNInt.hpp"

#include <complex>
#include <utility>
#include <stdexcept>

namespace CVNI_internal {
using std::complex;

#include <boost/math/quadrature/gauss_kronrod>

template <typename Real>
complex<Real> complex_from_c_complex(const Py_complex& c){
    return complex<Real>(c.real,c.imag);
}

bool convertable_to_py_complex(PyObject* obj){
    return PyComplex_Check(obj) || (PyObject_HasAttrString(obj, "__complex__") && PyCallableCheck(PyObject_GetAttrString(obj,"__complex__")));
}

template<typename Real>
class IntegrandFunctionWrapper {
    private:
        PyObject* callback = std::nullptr;
        PyObject* args = std::nullptr;
        PyObject* constructArgList(Real x){
            //TODO
        }
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
            
            if(callback == std::nullptr){
                throw std::logic_error("Attempted to call integrand function wrapper with no function defined.");
            }

            PyObject * arg_list = constructArgList(x); 
            
            PyObject * py_result = PyObject_CallObject(callback, arg_list);
            
            Py_DECREF(arg_list);

            if(py_result == NULL){
                throw PythonError("Error occured in integrand function");
            }
            
            if(convertable_to_py_complex(py_result)){
                Py_DECREF(py_result);
                throw std::invlid_argument("The return value of the integrand function could not be converted to a complex number");
            }

            complex<Real> cpp_result = complex_from_c_complex(PyComplex_AsCComplex(py_result);

            Py_DECREF(py_result);

            return cpp_result;
        }
};

PyObject* integrate_internal(

}
