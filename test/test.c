#define NULL (void *)0

#define METH_VARARGS  0x0001
#define METH_KEYWORDS 0x0002
/* METH_NOARGS and METH_O must not be combined with the flags above. */
#define METH_NOARGS   0x0004
#define METH_O        0x0008
#define METH_FASTCALL  0x0080
#define METH_METHOD 0x0200

typedef struct {} PyObject;
typedef PyObject *(*PyCFunction)(PyObject*, PyObject*);
typedef PyObject *(*getter)(PyObject *, void *);
typedef int (*setter)(PyObject *, PyObject *, void *);

typedef struct {
    const char *ml_name;
    PyCFunction ml_meth;
    int ml_flags;
    const char *ml_doc;
} PyMethodDef;

typedef struct {
    const char *name;
    getter get;
    setter set;
    const char* doc;
    void *closure;
} PyGetSetDef;

PyObject *myfunc(PyObject *a){
    return NULL;
}

PyObject *myfunc2(PyObject *a, PyObject *b){
    return NULL;
}
PyMethodDef my_func_decl;

PyMethodDef my_func_decl_list[] = { {
    .ml_name = "myfunc2",
    .ml_flags = METH_NOARGS,
    .ml_meth = (PyCFunction)myfunc
},
{0},
{NULL,NULL, 0, NULL}
};

PyMethodDef my_func_decl2 = {
    "myfunc",
    myfunc2,
    METH_NOARGS,
};

// PyMethodDef my_func_decl2 = {
//     "myfunc",
//     (PyCFunction)myfunc // missing comma
//     METH_NOARGS,
// };

PyObject*
arrayflags_updateifcopy_get(PyObject *self){
    return NULL;
}

int
arrayflags_updateifcopy_set(PyObject *self, PyObject *val){
    return 0;
}

PyGetSetDef blah = {"updateifcopy",
    (getter)arrayflags_updateifcopy_get,
    (setter)arrayflags_updateifcopy_set,
NULL, NULL};

PyGetSetDef blah2 = { "ah",
    .set = (setter)arrayflags_updateifcopy_set,
NULL, NULL};

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
