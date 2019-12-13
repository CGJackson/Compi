#include "kumquat.hpp"

#include <complex>
#include <utility>
#include <stdexcept>

#include "IntegrandFunctionWrapper.hpp"
#include "utils.hpp"

namespace kumquat_internal {
using std::complex;

IntegrandFunctionWrapper::IntegrandFunctionWrapper(const IntegrandFunctionWrapper& other)
            :callback{other.callback}, args{copy_py_tuple(other.args)} {
            if(args == NULL){
                throw unable_to_construct_wrapper("Unable to copy args in IntegrandFunctionWrapper");
            }
            Py_INCREF(other.callback);
        }

        //I can't think of a more efficent way to leave the orignial
        //in a valid state after moving than simply copying
IntegrandFunctionWrapper::IntegrandFunctionWrapper(IntegrandFunctionWrapper&& other)
            :callback{other.callback}, args{copy_py_tuple(other.args)} {
            if(args == NULL){
                throw unable_to_construct_wrapper("Unable to copy args in IntegrandFunctionWrapper");
            }
            Py_INCREF(other.callback);
        }
IntegrandFunctionWrapper::IntegrandFunctionWrapper(PyObject * func, 
                                        PyObject * new_args)
    :callback{func}{
    if( func == NULL){
        if(PyErr_Occurred() != NULL){//TODO check if this should be ==
                PyErr_SetString(PyExc_TypeError,"No Valid Python Object passed to IntegrandFunctionWrapper"); 
        }
        throw unable_to_construct_wrapper("Function passed to IntegrandFunctionWrapper cannot be NULL");
    }

    if(!PyCallable_Check(func)){
        throw function_not_callable("The Python Object for IntegrandFunctionWrapper to wrap was not callable", "Unable to wrap uncallable object");
    }
    Py_INCREF(func);

    if( new_args != nullptr ){
        if(!PyTuple_Check(new_args)){
            Py_DECREF(func);
            throw arg_list_not_tuple("The argument list given to IntegrandFunctionWrapper was not a Python Tuple", "The extra arguments passed to the function wrapper were not a valid python tuple");
        }

        const Py_ssize_t extra_arg_count = PyTuple_GET_SIZE(new_args);
        if(extra_arg_count == PY_SSIZE_T_MAX){
            PyErr_SetString(PyExc_TypeError,"Too many arguments provided to integrand function");
            throw unable_to_construct_wrapper("Too many arguments provided");
        }

        args = PyTuple_New(extra_arg_count+1);

        if( args == NULL){
            // If args == NULL, PyTuple_New failed and has set 
            // the python exception flags
            throw unable_to_construct_wrapper("error constructing fixed arg tuple");
        }

        PyTuple_SET_ITEM(args,0,Py_None);
        Py_INCREF(Py_None);
        for(Py_ssize_t i = 0; i < extra_arg_count; ++i){
            PyObject* fixed_arg = PyTuple_GET_ITEM(new_args,i);
            PyTuple_SET_ITEM(args,i+1,fixed_arg);
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

complex<Real> IntegrandFunctionWrapper::operator()(Real x){
    // Calls the Python function callback with x as a python float
    // and args as its other arguments and reutrns the result as a
    // std::complex
    

    PyObject* py_x = PyFloat_FromDouble(x);
    if(py_x == NULL){
        throw unable_to_construct_py_object("error converting callback arg to Py_Float");
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

    complex<Real> cpp_result = complex_from_c_complex(PyComplex_AsCComplex(py_result));

    Py_DECREF(py_result);

    return cpp_result;
}


}
