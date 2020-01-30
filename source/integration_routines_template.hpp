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
        Real l1;
    };

};
class could_not_parse_arguments: std::runtime_error{
    using std::runtime_error::runtime_error;
};

class unable_to_call_integration_routine: std::runtime_error{
    using std::runtime_error::runtime_error;
};

template<typename RoutineParameters>
PyObject* integration_routine(PyObject* args, PyObject* kwargs){
    using namespace::kumquat_internal;
    std::unique_ptr<const RoutineParameters> parameters;

    // The input Python Objects are parsed into c variables
    try{
        parameters = std::make_unique<const RoutineParameters>(args,kwargs);
    }catch(const could_not_parse_arguments& e){
        return NULL;
    }

    // C++ wrapper for Python integrand funciton is constructed
    
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

    // The actual integration routine is run

   typename RoutineParameters::result_type result; 
    try{
        result = run_integration_routine(*f,*parameters);
    } catch (const unable_to_call_integration_routine& e){
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

    // The results are parsed back to Python Objects 

    auto c_complex_result = c_complex_from_complex(result.result);

    if(parameters->full_output){
        PyObject* full_output_dict = generate_full_output_dict(result,*parameters);
        if(!full_output_dict){
            return NULL;
        }
        return Py_BuildValue("(DdO)", &c_complex_result, result.err,full_output_dict);
    }
    else{
        return Py_BuildValue("(Dd)", &c_complex_result,result.err);
    }

}

#endif
