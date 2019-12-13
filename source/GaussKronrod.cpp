#include "kumquat.hpp"

#include <complex>
#include <memory>

#include <boost/math/quadrature/gauss_kronrod.hpp>
#include <boost/math/tools/precision.hpp>

#include "integration_routines.hpp"
#include "IntegrandFunctionWrapper.hpp"
#include "utils.hpp"

extern "C" PyObject* gauss_kronrod(PyObject* self, PyObject* args){
    using std::complex;
    using namespace kumquat_internal;

    PyObject* integrand;

    Real x_min, x_max;

    PyObject* extra_args = nullptr;

    PyObject* extra_kw = nullptr;

    unsigned routine = 31;

    unsigned max_depth = 15;

    Real tolerance = boost::math::tools::root_epsilon<Real>();//TODO check if this is right
    
    if(!PyArg_ParseTuple(args,"Odd|OOIId",
                &integrand,&x_min,&x_max,
                extra_args,extra_kw,routine,max_depth,tolerance)){
        return NULL;
    }
    
    
    std::unique_ptr<IntegrandFunctionWrapper> f;
    try{
        f = std::make_unique<IntegrandFunctionWrapper>(integrand,extra_args);
    } catch( const unable_to_construct_wrapper& e ){
        return NULL;
    } catch( const function_not_callable& e ){
        return NULL;
    } catch( const arg_list_not_tuple& e ){
        return NULL;
    }

    complex<Real> result;
    Real err,l1;

    try{
        switch(routine){//TODO there must be a better way...
            case 15:
                result = boost::math::quadrature::gauss_kronrod<Real,15>::integrate(*f,x_min,x_max,max_depth,tolerance,&err,&l1);
                break;
            case 31:
                result = boost::math::quadrature::gauss_kronrod<Real,31>::integrate(*f,x_min,x_max,max_depth,tolerance,&err,&l1);
                break;
            case 41:
                result = boost::math::quadrature::gauss_kronrod<Real,41>::integrate(*f,x_min,x_max,max_depth,tolerance,&err,&l1);
                break;
            case 51:
                result = boost::math::quadrature::gauss_kronrod<Real,51>::integrate(*f,x_min,x_max,max_depth,tolerance,&err,&l1);
                break;
            case 61:
                result = boost::math::quadrature::gauss_kronrod<Real,61>::integrate(*f,x_min,x_max,max_depth,tolerance,&err,&l1);
                break;
            default:
                PyErr_SetString(PyExc_ValueError,"Invalid number of points for integrate");
                return NULL;
        }
    } catch( const unable_to_construct_py_object& e ){
        return NULL;
    } catch( const unable_to_form_arg_tuple& e ){
        return NULL;
    } catch( const PythonError& e ){
        return NULL;
    } catch( const function_did_not_return_complex& e ){
        return NULL;
    }

    //TODO do something with L1 norm 
    return Py_BuildValue("(Df)",c_complex_from_complex(result),err);
}
