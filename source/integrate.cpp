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

// Exceptions to be used in IntegrandFunctionWrapper`
class unable_to_construct_wrapper: public runtime_error{
    using std::runtime_error::runtime_error;
};
class function_not_callable: public std::invalid_argument{
    using std::invalid_argument::invalid_argument;
};
class arg_list_not_tuple: public std::invalid_argument{
    using std::invalid_argument::invalid_argument;
};
class function_did_not_return_complex: public std::invalid_argument{
    using std::invalid_argument::invalid_argument;
};

template<typename Real>
class IntegrandFunctionWrapper {
    private:
        PyObject* callback = std::nullptr;
        PyObject* args = std::nullptr;
        PyObject* constructArgList(Real x){
            if( args = std::nullptr){
               return Py_BuildValue("(d)", x);
            }

        }
    public:
        IntegrandFunctionWrapper(PyObject * func, PyObject * new_args = std::nullptr){
            :callback=func{
            if( func == std::nullptr){
                return;
            }

            if(!PyCallable_Check(func)){
                throw function_not_callable("The Python Object for IntegrandFunctionWrapper to wrap was not callable");
            }
            Py_INCREF(func);

            if( new_args != std::nullptr ){
                if(!PyTuple_Check(new_args)){
                    Py_DECREF(func);
                    throw arg_list_not_tuple("The argument list given to IntegrandFunctionWrapper was not a Python Tuple");
                }

                const Py_ssize_t extra_arg_count = Py_Tuple_GET_SIZE(new_args);
                if(extra_arg_count == PY_SSIZE_T_MAX){
                    PyErr_SetString(PyExc_TypeError,"Too many arguments provided to integrand function");
                    throw unable_to_construct_wrapper("Too many arguments provided");
                }

                args = PyTuple_New(extra_arg_count+1);

                if( args == NULL){
                    throw unable_to_construct_wrapper("error constructing fixed arg tuple");
                }

                Py_Tuple_SET_ITEM(args,0,Py_None);
                Py_INCREF(Py_None);
                for(Py_ssize_t i = 0; i < extra_arg_count; ++i){
                    PyObject* fixed_arg = PyTuple_GET_ITEM(new_args,i);
                    Py_Tuple_SET_ITEM(args,i+1,fixed_arg);
                    Py_INCREF(fixed_arg);
                }
            }
            else{
                args = Py_BuildValue("(s)", NULL);//args is a tuple of one value, which is set to None
                if( args == NULL ){
                    unable_to_construct_wrapper("error constructing arg tuple");
                }

            }
        }

        noexcept IntegrandFunctionWrapper(const IntegrandFunctionWrapper& other)
            :callback=other.callback, args=other.args {
            if( func != std::nullptr ){
                Py_INCREF(func);
            }
            if( args != std::nullptr ){
                Py_INCREF(args);
            }
        }

        noexcept IntegrandFunctionWrapper(IntegrandFunctionWrapper&& other): IntegrandFunctionWrapper() {
            swap(*this, other)
        }

        noexcept IntegrandFunctionWrapper& operator=(IntegrandFunctionWrapper other){
            swap(*this,other);
            return *this;
        }

        friend void noexcept swap(IntegrandFunctionWrapper& first, IntegrandFunctionWrapper& second){
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

            //TODO set first arg
            
            PyObject * py_result = PyObject_CallObject(callback, args);
            
            //TODO unset first arg

            if(py_result == NULL){
                throw PythonError("Error occured in integrand function");
            }
            
            if(convertable_to_py_complex(py_result)){
                Py_DECREF(py_result);
                throw function_did_not_return_complex("The return value of the integrand function could not be converted to a complex number");
            }

            complex<Real> cpp_result = complex_from_c_complex(PyComplex_AsCComplex(py_result);

            Py_DECREF(py_result);

            return cpp_result;
        }
};

PyObject* integrate_internal(

}
