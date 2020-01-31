#include "kumquat.hpp"

#include <complex>
#include <array>

#include <boost/math/quadrature/tanh_sinh.hpp>

#include "integration_routines_template.hpp"
#include "IntegrandFunctionWrapper.hpp"

extern "C" {
    #include "integration_routines.h"
}

struct TanhSinhParameters: public RoutineParametersBase {
    Real x_min;
    Real x_max;

    TanhSinhParameters(PyObject* routine_args, PyObject* routine_kwargs){
        std::array<const char*, standard_keywords.size()+3> keywords;
        auto keywords_it = keywords.begin();
        *keywords_it++ = standard_keywords[0];
        *keywords_it++ = "a";
        *keywords_it++ = "b";
        std::copy(standard_keywords.cbegin()+1, standard_keywords.end(),keywords_it);
        keywords.back() = NULL;

        if(!PyArg_ParseTupleAndKeywords(routine_args,routine_kwargs,"Odd|OO$IdI",const_cast<char**>(keywords.data()),
                &integrand,&x_min,&x_max,
                &args,&kw,
                &full_output, &max_levels,&tolerance)){
            throw could_not_parse_arguments("Unable to parse python arguments to C variables");
        }
    }

    struct result_type:public RoutineParametersBase::result_type {
        unsigned levels;
    };
};

TanhSinhParameters::result_type run_integration_routine(const kumquat_internal::IntegrandFunctionWrapper& f,const TanhSinhParameters& parameters){
    auto integrator = boost::math::quadrature::tanh_sinh(static_cast<size_t>(parameters.max_levels));
    TanhSinhParameters::result_type result;

    result.result =  integrator.integrate(f,parameters.x_min,parameters.x_max,parameters.tolerance,&(result.err),&(result.l1),&(result.levels));

    return result;
}

PyObject* generate_full_output_dict(const TanhSinhParameters::result_type& result,const TanhSinhParameters& parameters){
    return Py_BuildValue("{sdsI}","L1 norm",result.l1,"levels",result.levels);
}

extern "C" PyObject* tanh_sinh(PyObject* self, PyObject* args, PyObject* kwargs){
    return integration_routine<TanhSinhParameters>(args,kwargs);
}