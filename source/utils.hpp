#ifndef KUMQUAT_UTILS_GUARD
#define KUMQUAT_UTILS_GUARD
#include <complex>
#include "kumquat.hpp"

namespace kumquat_internal {

inline std::complex<Real> complex_from_c_complex(const Py_complex& c){
    return std::complex<Real>(c.real,c.imag);
}

inline Py_complex c_complex_from_complex(const std::complex<Real> c){
    return Py_complex{c.real(),c.imag()};
}

inline bool convertable_to_py_complex(PyObject* obj){
    return PyComplex_Check(obj) || (PyObject_HasAttrString(obj, "__complex__") && PyCallable_Check(PyObject_GetAttrString(obj,"__complex__")));
}

inline PyObject* copy_py_tuple(PyObject* tup){
    if(!PyTuple_Check(tup)){
        PyErr_SetString(PyExc_ValueError, "Object passed to copy_py_tuple was not a tuple");
        return NULL;
    }

    const Py_ssize_t size = PyTuple_Size(tup);

    PyObject* new_tup = PyTuple_New(size);
    if(new_tup == NULL){
        return NULL;
    }

    for(Py_ssize_t i = 0; i < size; ++i){
        PyObject* elm = PyTuple_GET_ITEM(tup, i);
        PyTuple_SET_ITEM(new_tup,i,elm);
        Py_INCREF(elm);
    }

    return new_tup;
}

}
#endif
