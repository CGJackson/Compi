#ifndef INTEGRATION_ROUTINES_TEMPLATE_GUARD
#define INTEGRATION_ROUTINES_TEMPLATE_GUARD

#include "kumquat.hpp"

#include <vector>
#include <complex>
#include <utility>
#include <stdexcept>

#include <boost/math/tools/precision.hpp>

#include "IntegrandFunctionWrapper.hpp"
#include "utils.hpp"

const std::vector<const char*> standard_keywords{"f","args", "kwargs", "full_output","max_levels", "tolerance"};

struct RoutineParametersBase{
    PyObject* integrand;
    PyObject* args = Py_None;
    PyObject* kw = Py_None;

    bool full_output = false;
    Real tolerance = boost::math::tools::root_epsilon<Real>();
    unsigned max_levels = 15;

    struct result_type{
        std::complex<Real> result;
        Real err;
    };

};
class could_not_parse_arguments: std::runtime_error{
    using std::runtime_error::runtime_error;
};

template<typename RoutineParameters>
PyObject* integration_routine(PyObject* args, PyObject* kwargs){

    std::unique_ptr<const RoutineParameters> parameters;

    try{
        parameters = make_unique<const RoutineParameters>(args,kwargs);
    }catch(const could_not_parse_arguments& e){
        return NULL;
    }
    
    std::unique_ptr<IntegrandFunctionWrapper> f;
    try{
        f = std::make_unique<IntegrandFunctionWrapper>(parameters->integrand,parameters->args,parameters->kw);
    } catch( const unable_to_construct_wrapper& e ){
        return NULL;
    } catch( const function_not_callable& e ){
        return NULL;
    } catch( const arg_list_not_tuple& e ){
        return NULL;
    } catch( const kwargs_given_not_dict& e){
        return NULL;
    } 

   RoutineParameters::result_type result; 
    try{
        result = run_integration_routine(parameters.get());
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

    auto c_complex_result = c_complex_from_complex(result.result);

    if(RoutineParameters.full_output){
        PyObject* full_output_dict = generate_full_output_dict(result,parameters.get());
        if(!full_output_dict){
            return NULL;
        }
        return Py_BuildValue("(DfO)", c_complex_result, result.err,full_output_dict);
    }
    else{
        return Py_BuildValue("(Df)", c_complex_result,result.err);
    }

}

#endif
