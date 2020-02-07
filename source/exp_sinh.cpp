#include "kumquat.hpp"

#include <vector>
#include <limits>

#include <boost/math/quadrature/exp_sinh.hpp>

extern "C" {
    #include "integration_routines.h"
}

#include "integration_routines_template.hpp"

struct ExpSinhParameters: public RoutineParametersBase {
    Real interval_end = 0.0;
    bool positive_axis;

    ExpSinhParameters(PyObject* routine_args,PyObject* routine_kwargs){
        auto keywords = generate_keyword_list(IntegralRange::semi_infinite,std::vector<const char*>{},std::vector<const char*>{"interval_infinity"});

        float sign = 1.0;

        if(!PyArg_ParseTupleAndKeywords(routine_args,routine_kwargs,"Od|OOf$pId",const_cast<char**>(keywords.data()),
                &integrand,&interval_end,
                &args,&kw,&sign,
                &full_output, &max_levels,&tolerance)){
            throw could_not_parse_arguments("Unable to parse python arguments to C variables");
        }
        positive_axis = sign > 0;
    }

    struct result_type: public RoutineParametersBase::result_type {
        size_t levels;
    };
};

ExpSinhParameters::result_type run_intgration_routine(const kumquat_internal::IntegrandFunctionWrapper& f, const ExpSinhParameters& parameters){
    static_assert(std::numeric_limits<Real>::has_infinity, "Real type does not have infinity");
    Real lower_bound, upper_bound;
    if(parameters.positive_axis){
        lower_bound = parameters.interval_end;
        upper_bound = std::numeric_limits<Real>::infinity();
    } else{
        upper_bound = parameters.interval_end;
        lower_bound = -std::numeric_limits<Real>::infinity();
    }

    boost::math::quadrature::exp_sinh<Real> integrator(parameters.max_levels);

    ExpSinhParameters::result_type result;

    result.result = integrator.integrate(f,lower_bound,upper_bound,parameters.tolerance,&(result.err),&(result.l1),&(result.levels));

    return result;
}

extern "C" PyObject* exp_sinh(PyObject* self, PyObject* args, PyObject* kwargs){
    return integration_routine<ExpSinhParameters>(args,kwargs);
}