#include "kumquat.hpp"

#include <complex>
#include <unordered_map>
#include <utility>

#include <boost/math/quadrature/gauss_kronrod.hpp>
#include <boost/math/tools/precision.hpp>

extern "C" {
    #include "integration_routines.h"
}
#include "IntegrandFunctionWrapper.hpp"
#include "utils.hpp"


template<unsigned points>
std::pair<PyObject*, PyObject*> get_abscissa_and_weights(){
    auto abscissa = kumquat_internal::py_list_from_real_container(boost::math::quadrature::gauss_kronrod<Real,points>::abscissa());
    if(abscissa == NULL){
        return std::make_pair<PyObject*,PyObject*>(NULL,NULL);
    }
    auto weights = kumquat_internal::py_list_from_real_container(boost::math::quadrature::gauss_kronrod<Real,points>::weights());
    if(weights == NULL){
        Py_DECREF(abscissa);
        return std::make_pair<PyObject*,PyObject*>(NULL,NULL);
    }
    return std::make_pair(abscissa,weights);
}

// Integrates a Python function returning a complex over a finite interval using
// Gauss-Kronrod adaptive quadrature
extern "C" PyObject* gauss_kronrod(PyObject* self, PyObject* args, PyObject* kwargs){
    using std::complex;
    using namespace kumquat_internal;

    using IntegrationRoutine = complex<Real>(*)(IntegrandFunctionWrapper, Real, Real, unsigned, Real, Real*, Real*);


    // The input Python Objects are parsed into c variables

    PyObject* integrand;

    Real x_min, x_max;

    PyObject* extra_args = Py_None;

    PyObject* extra_kw = Py_None;

    unsigned routine = 31;

    unsigned max_depth = 15;

    Real tolerance = boost::math::tools::root_epsilon<Real>();//TODO check if this is right

    bool full_output = false;
    
    const char* keywords[] = {"","","","args", "kwargs", "full_output","max_levels", "tolerance","points", NULL}; 

    if(!PyArg_ParseTupleAndKeywords(args,kwargs,"Odd|OO$pIdI",const_cast<char**>(keywords),
                &integrand,&x_min,&x_max,
                &extra_args,&extra_kw
                ,&full_output, &max_depth,&tolerance,&routine)){
        return NULL;
    }

    // C++ wrapper for Python integrand funciton is constructed

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

    // The actual integration routine is run

        // The possible tempates for the different allowed numbers of divisions are instasiated, 
        // so that the Python runtime can select which one to use
    static const std::unordered_map<unsigned,IntegrationRoutine> integration_routines{{15,boost::math::quadrature::gauss_kronrod<Real,15>::integrate},
                                                                                      {31,boost::math::quadrature::gauss_kronrod<Real,31>::integrate},
                                                                                      {41,boost::math::quadrature::gauss_kronrod<Real,41>::integrate},
                                                                                      {51,boost::math::quadrature::gauss_kronrod<Real,51>::integrate},
                                                                                      {61,boost::math::quadrature::gauss_kronrod<Real,61>::integrate}
                                                                                     };

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


    // The results are parsed back to Python Objects 
    auto c_complex_result = c_complex_from_complex(result);

    if(full_output){

        // The boost API returning the abscissa and weights simply spesifies that
        // they are returned as a (reference to a) random access container. 
        // Since this could (reasonably) depend on points (e.g. array<Real,points>)
        // it is awkward to use the map based appraoch used for calling the routine
        // and instead we use more verbose a template/switch based approach

        std::pair<PyObject*,PyObject*> abscissa_and_weights;
        switch(routine){
            case 15:
                abscissa_and_weights = get_abscissa_and_weights<15>();
                break;
            case 31:
                abscissa_and_weights = get_abscissa_and_weights<31>();
                break;
            case 41:
                abscissa_and_weights = get_abscissa_and_weights<41>();
                break;
            case 51:
                abscissa_and_weights = get_abscissa_and_weights<51>();
                break;
            case 61:
                abscissa_and_weights = get_abscissa_and_weights<61>();
                break;
            default:
                // Should never get here as have already checked the value of points is valid 
                PyErr_SetString(PyExc_NotImplementedError,"Unable to generate abscissa and weights for the given number of points.\nPlease report this bug");
                return NULL;
        }

        if(abscissa_and_weights == std::pair<PyObject*,PyObject*>(NULL,NULL)){
            return NULL;
        }

        return Py_BuildValue("(Df{sfsOsO})",&c_complex_result,err,"L1 norm",l1,"abscissa",abscissa_and_weights.first,"weights",abscissa_and_weights.second);
    }
    else{
        return Py_BuildValue("(Df)",&c_complex_result,err);
    }
}
