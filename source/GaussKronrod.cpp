#include <complex>

#include "kumquat.hpp"
#include "utils.hpp"

extern "C" PyObject* integrate(PyObject* self, PyObject* args, PyObject* kw){
    using std::complex;
    using namespace kumquat_internal;

    PyObject* integrand;

    Real_T x_min, x_max;

    PyObject* extra_args = std:nullptr;

    PyObject* extra_kw = std::nullptr;

    unsigned routine = 31;

    unsigned max_depth = 15;

    Real_T tolerance = boost::tools::root_epsilon<Real_T>();//TODO check if this is right
    
    if(!PyArg_ParseArgsAndKeywords(args,kw,"Odd|OOIId",
                integrand,&x_min,&x_max,
                extra_args,extra_kw,routine,max_depth,tolerance)){
        return NULL;
    }
    
    

    try{
        IntegrandFunctionWrapper f{integrand,extra_args};
    }
    catch const &unable_to_construct_wrapper e {
        return NULL;
    }
    catch const &function_not_callable e {
        return NULL;
    }
    catch const &arg_list_not_tuple e {
        return NULL;
    }

    complex<Real_T> result;
    Real_T err,l1;

    try{
        switch(routine){//TODO there must be a better way...
            case 15:
                result = boost::math::gauss_kronrod<Real_T,15>(f,x_min,x_max,max_depth,tolerance,&err,&l1);
                break;
            case 31:
                result = boost::math::gauss_kronrod<Real_T,31>(f,x_min,x_max,max_depth,tolerance,&err,&l1);
                break;
            case 41:
                result = boost::math::gauss_kronrod<Real_T,41>(f,x_min,x_max,max_depth,tolerance,&err,&l1);
                break;
            case 51:
                result = boost::math::gauss_kronrod<Real_T,51>(f,x_min,x_max,max_depth,tolerance,&err,&l1);
                break;
            case 61:
                result = boost::math::gauss_kronrod<Real_T,61>(f,x_min,x_max,max_depth,tolerance,&err,&l1);
                break;
            default:
                PyErr_SetString(Py_ValueError,"Invalid number of points for integrate");
                return NULL;
        }
    }
    catch const &unable_to_construct_py_object e {
        return NULL;
    }
    catch const &unable_to_form_arg_tuple e {
        return NULL;
    }
    catch const &PythonError e {
        return NULL;
    }
    catch const &function_did_not_return_complex e {
        return NULL;
    }

    //TODO do something with L1 norm 
    return PyBuildValue("(Df)",c_complex_from_complex(result),err);
}
