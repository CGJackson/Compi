#ifndef KUMQUAT_INTEGRAND_FUNCTION_WRAPPER_GUARD
#define KUMQUAT_INTEGRAND_FUNCTION_WRAPPER_GUARD

#include <kumquat.hpp>


namespace kumquat_internal {

// Exceptions to be used in IntegrandFunctionWrapper
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

class IntegrandFunctionWrapper {//TODO handle keyword args
    private:
        PyObject* callback;
        PyObject* args;
        }
        
    public:
        IntegrandFunctionWrapper() = delete;
        IntegrandFunctionWrapper(PyObject * func, PyObject * new_args = std::nullptr);
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

        complex<Real> operator()(Real x);
};
}

#endif
