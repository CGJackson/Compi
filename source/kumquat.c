#include "kumquat.hpp"
#include "integration_routines.h"
#include "doc_strings.h"

/* Method Table */
static PyMethodDef KumquatMethods[] = {
    {"trapezoidal", (PyCFunction) trapezoidal, METH_VARARGS | METH_KEYWORDS,
    TRAPEZOIDAL_DOCS},
    {"gauss_kronrod", (PyCFunction) gauss_kronrod, METH_VARARGS | METH_KEYWORDS,
    GAUSS_KRONROD_DOCS },
    {"tanh_sinh", (PyCFunction) tanh_sinh, METH_VARARGS | METH_KEYWORDS,
    TANH_SINH_DOCS},
    {"sinh_sinh", (PyCFunction) sinh_sinh, METH_VARARGS | METH_KEYWORDS,
    SINH_SINH_DOCS},
    {"exp_sinh", (PyCFunction) exp_sinh, METH_VARARGS | METH_KEYWORDS,
    EXP_SINH_DOCS},
    {NULL,NULL,0,NULL}
};

/* Module definition Structure */
static struct PyModuleDef KumquatModule = {
    PyModuleDef_HEAD_INIT,
    "kumquat",/* Module name */
    KUMQUAT_DOCS, 
    -1, /* Module keeps state at global scope */
    KumquatMethods
};

/* Module initialization function */
PyMODINIT_FUNC PyInit_kumquat(void){
    return PyModule_Create(&KumquatModule);
}
