# Clang Python Wasm Plugin
A clang plugin to fix up common function pointer casts in Python C code. Thanks
to [clang-tutor](https://github.com/banach-space/clang-tutor) for helping me to
understand.

## Work in progress

Still need to figure out how to use this in Pyodide build process
(see [the fpcast removal PR](https://github.com/pyodide/pyodide/pull/1677)).

## Why?

There are two types of function pointer casting that are common in Python code
that we want to detect and patch.
```C
PyObject*
some_method(PyObject* self){
    // method body
}

// ... later
PyMethodDef {
    .ml_name = "myfunc2",
    .ml_flags = METH_NOARGS,
    .ml_meth = (PyCFunction)myfunc
}
```
The problem: `some_method` is a `METH_NOARGS` method. These should have
signature `PyObject* some_method(PyObject* self, PyObject* always_null)`. The
actual call occurs here:
https://github.com/python/cpython/blob/main/Objects/methodobject.c#L489 and
looks like `PyObject *result = meth(PyCFunction_GET_SELF(func), NULL);` In x86,
it's okay to cast `some_method` to a function of two arguments and call it with
a second argument. The second argument will be ignored and everything works
fine. In wasm, this causes a function signature mismatch trap. We need to patch
`some_method` to take a second argument.

The second common problem is with getters and setters:
```C
PyObject*
arrayflags_updateifcopy_get(PyObject *self){
    return NULL;
}

int
arrayflags_updateifcopy_set(PyObject *self, PyObject *val){
    return 0;
}

PyGetSetDef blah = {
    "updateifcopy",                      // char* name;
    (getter)arrayflags_updateifcopy_get, // getter get;
    (setter)arrayflags_updateifcopy_set, // setter set;
    NULL,  // char* doc
    NULL   // void* closure
};
```
The type signature for `getter` is
`PyObject* func(PyObject *self, void *closure)`.
The type signature for `setter` is
`PyObject* func(PyObject *self, PyObject *value, void *closure)`.

The last field of `PyGetSetDef` is a "closure" which is always passed as a last
argument to the getter and setter. In many examples, this closure argument goes
unused, and so people leave it out entirely. Again, we get function signature
mismatch traps at runtime.


## Installing clang 13 / llvm 13
We first need to install clang 13. I think it's desirable to avoid building it,
though that is of course possible. There are instructions here:
https://apt.llvm.org/

I think the specific invocation that worked for me was:
```sh
wget https://apt.llvm.org/llvm.sh
chmod +x llvm.sh
sudo ./llvm.sh 13
apt-get install llvm-13-dev clang-13 libclang-13-dev
```

## Building

I build this as follows:
```sh
mkdir build
cmake -DClang_INSTALL_DIR=/usr/lib/llvm-13/ ..
make
```

## Running
Once you've built the plugin, from the root directory you can run it as:
```sh
clang -cc1 -load build/lib/libMyPlugin.so -plugin LAC test/test.c
```
If you get an error like
`error: unable to load plugin 'lib/libMyPlugin.so': 'lib/libMyPlugin.so: undefined symbol`,
you are probably using the wrong version of clang.
Try `clang --version` and make sure it's version 13.
