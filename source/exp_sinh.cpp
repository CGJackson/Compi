#include "kumquat.hpp"

#include <array>
#include <limits>

#include <boost/version.hpp>
#include <boost/math/quadrature/exp_sinh.hpp>

extern "C" {
    #include "integration_routines.h"
}

#include "integration_routines_template.hpp"

struct ExpSinhParameters: public RoutineParametersBase {
    Real interval_end = 0.0;
    bool positive_axis;

    ExpSinhParameters(PyObject* routine_args,PyObject* routine_kwargs){
        using std::array;
        constexpr array<const char*,0> dumby {};
        constexpr auto keywords = generate_keyword_list<IntegralRange::semi_infinite>(dumby,dumby,array<const char*,1>{"interval_infinity"});

        float sign = 1.0;

        if(!PyArg_ParseTupleAndKeywords(routine_args,routine_kwargs,"Od|OOf$pId",const_cast<char**>(keywords.data()),
                &integrand,&interval_end,
                &args,&kw,&sign,
                &full_output, &max_levels,&tolerance)){
            throw could_not_parse_arguments("Unable to parse python arguments to C variables");
        }
        //TODO raise value error if sign == 0
        positive_axis = sign > 0;
    }

    struct result_type: public RoutineParametersBase::result_type {
        size_t levels;
    };
};

ExpSinhParameters::result_type run_integration_routine(const kumquat_internal::IntegrandFunctionWrapper& f, const ExpSinhParameters& parameters){
    static_assert(std::numeric_limits<Real>::has_infinity, "Real type does not have infinity");
    using std::complex;
    boost::math::quadrature::exp_sinh<Real> integrator(parameters.max_levels);

    ExpSinhParameters::result_type result;

    // Work around bug in early versions of boost where exp_sinh.integrate will not compile when used with a complex valued function over a non-native range
    #if BOOST_VERSION > 999999999 // larger than any value in current boost versioning scheme, as there is no version yet where this bug is fixed... 
        Real lower_bound, upper_bound;
        if(parameters.positive_axis){
            lower_bound = parameters.interval_end;
            upper_bound = std::numeric_limits<Real>::infinity();
        }
        else{
            upper_bound = parameters.interval_end;
            lower_bound = -std::numeric_limits<Real>::infinity();
        }
        result.result = integrator.integrate(f,lower_bound, upper_bound,parameters.tolerance,&(result.err),&(result.l1),&(result.levels));
    #else
        // maps the function f onto the native range of the exp_sinh integrator (0,oo)
        auto f_shifted = [
                            &f, 
                            sign=(parameters.positive_axis ? static_cast<Real>(1):static_cast<Real>(-1)),
                            shift=parameters.interval_end
                         ](Real x){ 
                             return f(sign*x + shift);
                         };

        result.result = integrator.integrate(f_shifted,parameters.tolerance,&(result.err),&(result.l1),&(result.levels));
    #endif
    return result;
}

extern "C" PyObject* exp_sinh(PyObject* self, PyObject* args, PyObject* kwargs){
    return integration_routine<ExpSinhParameters>(args,kwargs);
}