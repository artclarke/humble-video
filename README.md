This is the humble-video project. 

This project builds native code that can be used for Humble Software, and
also generates Java files that can be compiled into Maven libraries
that use the native code.

To build the native code the autotools stack is required:

* GNU c++ compilers
* YASM (for assembly)
* The Xuggle-Swig version of Swig (Use https://github.com/artclarke/xuggle-swig to get JavaDoc generated). If you can't do that, you must use Swig 1.3.
* Doxygen & DOT (for C++ documentation)
* Valgrind (for auto memory leak detection -- try 'make memcheck')
* Java JDK 1.6 or higher

The 'brew' utility is your on Mac OS X friend. Use it often.

Oddly enough, you'll also need (soon) Oracle's VirtualBox installed as well.

The build system is very new and not yet 'repeatable' easily across environments. My design goal is to make it trivial for users of humble-video to incorporate the Jar files, but this will come at the expense of making it harder for people building.

Also so far I only have Mac OSX builds working (for a variety of reasons, that will be the primary dev platform)
* Create an environment variable called HUMBLE_HOME to point to where you want to stage all native code (it will later be bundled into Jar files, but for building it's got to go somewhere).
* Build and install the Captive libraries
** Git clone the https://github.com/artclarke/humble-captive project (let's call this directory CAPTIVE_SRC)
** Create a separate (out-of-tree) directory where you are will build the captive code. Let's calls this CAPTIVE_BUILD
** Run $(CAPTIVE_SRC)/cross-compile.sh in the $(CAPTIVE_BUILD) directory.
* Build and install the Humble libraries
** More to come...

Enjoy!

- Art
