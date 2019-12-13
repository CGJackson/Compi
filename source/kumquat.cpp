#include "kumquat.hpp"
#include "integration_routines.hpp"

// Method Table
static PyMethodDef KumquatMethods[] = {
    {"gauss_kronrod", gauss_kronrod, METH_VARARGS|METH_KEYWORDS,
       "Performs gauss-kronrod integration"},
    {NULL,NULL,0,NULL}
};

char * kumquat_docs = NULL;

// Module definition Structure
static struct PyModuleDef KumquatModule = {
    PyModuleDef_HEAD_INIT,
    "kumquat",// Module name
    kumquat_docs,
    -1, // Module keeps state at global scope
    KumquatMethods
};

// Module initialization function
PyMODINIT_FUNC PyInit_kumquat(void){
    return PyModule_Create(&KumquatModule);
}
