#ifndef KUMQUAT_MAIN_HEADER_GUARD
#define KUMQUAT_MAIN_HEADER_GUARD

#define PY_SSIZE_T_CLEAN
#include <Python.h>

typedef Real double;

extern "C" static PyObject* integrate(PyObject* self, PyObject* args, PyObject* kw);

#endif
