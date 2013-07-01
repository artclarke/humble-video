This is the humble-video project. 

This project builds native code that can be used for Humble Software, and
also generates Java files that can be compiled into Maven libraries
that use the native code.

To build the native code the autotools stack is required:

* GNU c++ compilers
* YASM (for assembly)
* SWIG (Use xuggle-swig, available on GitHub, to get JavaDoc generated)
* Doxygen & DOT (for C++ documentation)
* Valgrind (for auto memory leak detection -- try 'make memcheck')
* Java JDK 1.6 or higher

Enjoy!

- Art
