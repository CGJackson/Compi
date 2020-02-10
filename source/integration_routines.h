#ifndef KUMQUAT_INTEGRATION_ROUTINES_GUARD
#define KUMQUAT_INTEGRATION_ROUTINES_GUARD

#include "kumquat.hpp"

PyObject* gauss_kronrod(PyObject* self, PyObject* args, PyObject* kwargs);

PyObject* tanh_sinh(PyObject* self, PyObject* args, PyObject* kwargs);

PyObject* sinh_sinh(PyObject* self, PyObject* args, PyObject* kwargs);

PyObject* trapezoidal(PyObject* self, PyObject* args, PyObject* kwargs);
#endif
