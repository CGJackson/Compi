#include "kumquat.hpp"

#include <complex>
#include <unordered_map>
#include <memory>

#include <boost/math/quadrature/gauss_kronrod.hpp>
#include <boost/math/tools/precision.hpp>

#include "integration_routines.hpp"
#include "IntegrandFunctionWrapper.hpp"
#include "utils.hpp"

extern "C" PyObject* gauss_kronrod(PyObject* self, PyObject* args){
    using std::complex;
    using namespace kumquat_internal;

    // The posible tempates for the different allowed numbers of divisions are instasiated, so that the Python runtime can select which one to use
    using IntegrationRoutine = complex<Real>(*)(IntegrandFunctionWrapper, Real, Real, unsigned, Real, Real*, Real*);

    static const std::unordered_map<unsigned,IntegrationRoutine> integration_routines{{15,boost::math::quadrature::gauss_kronrod<Real,15>::integrate},
                                                                                      {31,boost::math::quadrature::gauss_kronrod<Real,31>::integrate},
                                                                                      {41,boost::math::quadrature::gauss_kronrod<Real,41>::integrate},
                                                                                      {51,boost::math::quadrature::gauss_kronrod<Real,51>::integrate},
                                                                                      {61,boost::math::quadrature::gauss_kronrod<Real,61>::integrate}
                                                                                     };

    PyObject* integrand;

    Real x_min, x_max;

    PyObject* extra_args = Py_None;

    PyObject* extra_kw = Py_None;

    unsigned routine = 31;

    unsigned max_depth = 15;

    Real tolerance = boost::math::tools::root_epsilon<Real>();//TODO check if this is right
    
    if(!PyArg_ParseTuple(args,"Odd|OOIId",
                &integrand,&x_min,&x_max,
                &extra_args,&extra_kw,&routine,&max_depth,&tolerance)){
        return NULL;
    }

    std::unique_ptr<IntegrandFunctionWrapper> f;
    try{
        f = std::make_unique<IntegrandFunctionWrapper>(integrand,extra_args,extra_kw);
    } catch( const unable_to_construct_wrapper& e ){
        return NULL;
    } catch( const function_not_callable& e ){
        return NULL;
    } catch( const arg_list_not_tuple& e ){
        return NULL;
    } catch( const kwargs_given_not_dict& e){
        return NULL;
    } 

    complex<Real> result;
    Real err,l1;

    try{
        result = integration_routines.at(routine)(*f,x_min,x_max,max_depth,tolerance,&err,&l1);
    } catch (const std::out_of_range& e){
        PyErr_SetString(PyExc_ValueError,"Invalid number of points for gauss_kronrod");
        return NULL;
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
    auto c_complex_result = c_complex_from_complex(result);

    return Py_BuildValue("(Df)",&c_complex_result,err);
}
