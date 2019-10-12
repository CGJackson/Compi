#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "ComplexValuedNInt.h"

// Method Table
static PyMethodDef ComplexValuedNIntMethods[] = {
    {NULL,NULL,0,NULL}
};

char * complex_valued_nint_docs = NULL;

// Module definition Structure
static struct PyModuleDef ComplexValuedNIntModule = {
    PyModuleDef_HEAD_INIT,
    "complex_valued_nint",// Module name
    complex_valued_nint_docs,
    -1, // Module keeps state at global scope
    ComplexValuedNIntMethods
};

// Module initialization function
PyMODINIT_FUNC PyInit_complex_valued_nint(void){
    return ComplexValuedNIntModule;
}
