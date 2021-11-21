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