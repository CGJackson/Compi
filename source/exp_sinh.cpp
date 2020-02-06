#include "kumquat.hpp"

extern "C" {
    #include "integration_routines.h"
}

#include "integration_routines_template.hpp"

struct ExpSinhParameters: public RoutineParametersBase {
    Real interval_end = 0.0;
    bool positive_axis = true;

    struct result_type: public RoutineParametersBase::result_type {
        size_t levels;
    };
};

extern "C" PyObject* exp_sinh(PyObject* self, PyObject* args, PyObject* kwargs){
    return integration_routine<ExpSinhParameters>(args,kwargs);
}