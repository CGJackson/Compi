#ifndef KUMQUAT_INTEGRATION_ROUTINES_TEMPLATE_GUARD
#define KUMQUAT_INTEGRATION_ROUTINES_TEMPLATE_GUARD

#include "kumquat.hpp"

#include <vector>
#include <complex>
#include <utility>
#include <stdexcept>

#include <boost/math/tools/precision.hpp>

#include "IntegrandFunctionWrapper.hpp"
#include "utils.hpp"

enum class IntegralRange: short unsigned {infinite, semi_infinite, finite};

// Generates a vector of the standard keywords, appropriate bounds depending
// on the integration bounds, and any extra required, optional and keyword
// only arguments, in the correct order to be used in Py_ParseTupleAndKeywords
// for an integration routine.
inline std::vector<const char *> generate_keyword_list(IntegralRange bounds, const std::vector<const char*>& required = std::vector<const char*>{}, const std::vector<const char*> optional = std::vector<const char*>{}, const std::vector<const char*> keyword_only = std::vector<const char*>{}){
    
    std::vector<const char*> keywords{"f"};

    switch(bounds){
        case IntegralRange::finite:
            keywords.push_back("a");
        case IntegralRange::semi_infinite:
            keywords.push_back("b");
        case IntegralRange::infinite:;
    }

    keywords.insert(keywords.end(),required.begin(),required.end());

    keywords.push_back("args");
    keywords.push_back("kwargs");

    keywords.insert(keywords.end(),optional.begin(),optional.end());

    keywords.push_back("full_output");
    keywords.push_back("max_levels");
    keywords.push_back("tolerance");

    keywords.insert(keywords.end(),keyword_only.begin(),keyword_only.end());

    keywords.push_back(NULL);

    return keywords;
}

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

    decltype(run_integration_routine(*f,*parameters)) result;
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
template<typename ExpIntegratorParameterType,typename ExpIntegratorResultType>
PyObject* generate_full_output_dict(const ExpIntegratorResultType& result,const ExpIntegratorParameterType& parameters){
    return Py_BuildValue("{sdsI}","L1 norm",result.l1,"levels",result.levels);
}
#endif
