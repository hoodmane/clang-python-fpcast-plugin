// RUN: clang -cc1 -load %S/../build/lib/libPyFpcastRemover.so -plugin pyfpcast %s 2>&1 | FileCheck %s

#include "python_stubs.h"

// CHECK-LABEL: lbl1
// CHECK: arrayflags_updateifcopy_get(PyObject *self, void *ignore{{d}})
PyObject*
arrayflags_updateifcopy_get(PyObject *self){
    return NULL;
}

// CHECK-LABEL: lbl2
// CHECK: arrayflags_updateifcopy_set(PyObject *self, PyObject *val, void *ignore{{d}})
int
arrayflags_updateifcopy_set(PyObject *self, PyObject *val){
    return 0;
}
// CHECK-LABEL: lbl3

PyGetSetDef blah = {"updateifcopy",
    (getter)arrayflags_updateifcopy_get,
    (setter)arrayflags_updateifcopy_set,
NULL, NULL};

// PyGetSetDef blah2 = { "ah",
//     .set = (setter)arrayflags_updateifcopy_set,
// NULL, NULL};

// PyMethodDef my_func_decl3 = {
//     .meth = (PyCFunction)myfunc
//     .meth_type = METH_NOARGS,
// };

// PyMethodDef my_func_decl3 = {
//     .meth = (PyCFunction)myfunc,
//     .meth_type = METH_NOARGS,
// };

// PyMethodDef my_func_decl2 = {
//     .meth_type = METH_NOARGS,
//     .meth = (PyCFunction)myfunc2
// };


PyMethodDef my_func_decl2;
