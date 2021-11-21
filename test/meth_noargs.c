// RUN: clang -cc1 -load %S/../build/lib/libPyFpcastRemover.so -plugin pyfpcast %s 2>&1 | FileCheck %s

#include "python_stubs.h"

// CHECK-LABEL: lbl1
// CHECK: myfunc(PyObject *a, PyObject *ignore{{d}})
static PyObject *myfunc(PyObject *a){
    return NULL;
}

// CHECK-LABEL: lbl2
// CHECK: myfunc2(PyObject *a, PyObject *{{b}})
PyObject *myfunc2(PyObject *a, PyObject *b){
    return NULL;
}

// CHECK-LABEL: lbl3
// CHECK: myfunc3(PyObject *a, PyObject *b, PyObject *ignore{{d}})
PyObject *myfunc3(PyObject *a, PyObject *b){
    return NULL;
}
// CHECK-LABEL: lbl4

// M
PyMethodDef my_func_decl;
static PyMethodDef my_func_decl_list[];

static PyMethodDef my_func_decl_list[] = { {
    .ml_name = "myfunc",
    .ml_flags = METH_NOARGS,
    .ml_meth = (PyCFunction)myfunc
},
{
    .ml_name = "myfunc",
    .ml_flags = METH_VARARGS | METH_KEYWORDS,
    .ml_meth = (PyCFunction)myfunc3
},
{0},
{NULL,NULL, 0, NULL}
};

PyMethodDef my_func_decl2 = {
    "myfunc2",
    myfunc2,
    METH_NOARGS,
};
