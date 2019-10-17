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

PyObject* copy_py_tuple(PyObject* tup){
    if(!PyTupleCheck(tup)){
        PyErr_SetString(PyExc_ValueError, "Object passed to copy_py_tuple was not a tuple");
        return NULL;
    }

    const Py_ssize_t size = PyTuple_Size(tup)

    PyObject* new_tup = PyTuple_New(size);
    if(new_tup == NULL){
        return NULL;
    }

    for(Py_ssize_t i = 0; i < size; ++i){
        PyObject* elm = PyTuple_GET_ITEM(tup, i);
        PyTuple_SET_ITEM(new_tup,i,elm);
        Py_INCREF(elm);
    }

    return new_tup;
}

// Exceptions to be used in IntegrandFunctionWrapper`
class unable_to_construct_wrapper: public std::runtime_error{
    using std::runtime_error::runtime_error;
};
class unable_to_construct_py_object: public std::runtime_error{
    using std::runtime_error::runtime_error;
};
class function_not_callable: public std::invalid_argument{
    using std::invalid_argument::invalid_argument;
    function_not_callable(const char* c_message, const char* python_message):invalid_argument(c_message){
        PyErr_SetString(PyExc_ValueError,python_message);
    }
};
class arg_list_not_tuple: public std::invalid_argument{
    using std::invalid_argument::invalid_argument;
    arg_list_not_tuple(const char * c_message, const char* python_message): std::invalid_argument(c_message){
        PyErr_SetString(PyExc_ValueError,python_message);
    }
};
class function_did_not_return_complex: public std::invalid_argument{
    using std::invalid_argument::invalid_argument;
    function_did_not_return_complex(const char * c_message, const char* python_message): std::invalid_argument(c_message){
        PyErr_SetString(PyExc_ValueError,python_message);
    }
};
class unable_to_form_arg_tuple: public std::runtime_error{
    using std::runtime_error::runtime_error;
};

template<typename Real>
class IntegrandFunctionWrapper {
    private:
        PyObject* callback;
        PyObject* args;
        }
        
    public:
        IntegrandFunctionWrapper() = delete;
        IntegrandFunctionWrapper(PyObject * func, PyObject * new_args = std::nullptr){
            :callback=func{
            if( func == NULL){
                if(PyErr_Occured() != NULL){
                    PyErr_SetString(PyExc_TypeError,"No Valid Python Object passed to IntegrandFunctionWrapper"); 
                }
                throw unable_to_construct_wrapper("Function passed to IntegrandFunctionWrapper cannot be NULL");
            }

            if(!PyCallable_Check(func)){
                throw function_not_callable("The Python Object for IntegrandFunctionWrapper to wrap was not callable", "Unable to wrap uncallable object");
            }
            Py_INCREF(func);

            if( new_args != std::nullptr ){
                if(!PyTuple_Check(new_args)){
                    Py_DECREF(func);
                    throw arg_list_not_tuple("The argument list given to IntegrandFunctionWrapper was not a Python Tuple", "The extra arguments passed to the function wrapper were not a valid python tuple");
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

        IntegrandFunctionWrapper(const IntegrandFunctionWrapper& other)
            :callback=other.callback, args=copy_py_tuple(other.args) {
            if(args == NULL){
                throw unable_to_construct_wrapper("Unable to copy args in IntegrandFunctionWrapper");
            }
            Py_INCREF(func);
        }

        //I can't think of a more efficent way to leave the orignial
        //in a valid state after moving than simply copying
        IntegrandFunctionWrapper(IntegrandFunctionWrapper&& other)
            :callback=other.callback, args=copy_py_tuple(other.args) {
            if(args == NULL){
                throw unable_to_construct_wrapper("Unable to copy args in IntegrandFunctionWrapper");
            }
            Py_INCREF(func);
        }

        IntegrandFunctionWrapper& operator=(IntegrandFunctionWrapper other){
            swap(*this,other);
            return *this;
        }

        // similar to move constructor, the most efficent thing to do here
        // simply seems to be to swap elements
        IntegrandFunctionWrapper& operator=(IntegrandFunctionWrapper&& other){
            swap(*this,other);
            return *this;
        }

        friend void noexcept swap(IntegrandFunctionWrapper& first, IntegrandFunctionWrapper& second){
            using std::swap;
            swap(first.callback,second.callback);
            swap(first.args,second.args);
        }

        ~IntegrandFunctionWrapper(){
            Py_DECREF(func);
            Py_DECREF(args);
        }

        complex<Real> operator()(Real x){
            // Calls the Python function callback with x as a python float
            // and args as its other arguments and reutrns the result as a
            // std::complex
            

            PyObject* py_x = PyFloat_FromDouble(x);
            if(py_x == NULL){
                throw unable_to_construct_py_object("error converting callback arg to Py_Float")
            }
            if(!PyTuple_SetItem(args,0,py_x)){
                throw unable_to_form_arg_tuple("unable to add new x to arg list");
            }
            Py_DECREF(Py_None);
            
            PyObject * py_result = PyObject_CallObject(callback, args);
            
            if(!PyTuple_SetItem(args,0,Py_None)){
                throw unable_to_form_arg_tuple("unable to remove new x from arg list");
            }
            Py_INCREF(Py_None);
            Py_DECREF(py_x);

            if(py_result == NULL){
                throw PythonError("Error occured in integrand function");
            }
            
            if(convertable_to_py_complex(py_result)){
                Py_DECREF(py_result);
                throw function_did_not_return_complex("The return value of the integrand function could not be converted to a complex number", 
                        "The function passed to IntegrandFunctionWrapper did not return a value that culd be converted to complex");
            }

            complex<Real> cpp_result = complex_from_c_complex(PyComplex_AsCComplex(py_result);

            Py_DECREF(py_result);

            return cpp_result;
        }
};

PyObject* integrate_internal(

}
