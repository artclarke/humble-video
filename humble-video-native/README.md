The `humble-video-native` contains all the native C and C++ code for Humble Video.

The source can be found under `src/main/gnu`. It creates wrapper objects around the excellent
[http://ffmpeg.org/](FFMPEG) project, and makes them objects look like (mostly) Java objects.

This involves some fairly complicated semantics with memory and exception handling, and then the use of [https://github.com/artclarke/humble-swig](Humble Swig) to create the Java classes automatically in the `humble-video-noarch` project that call into native code.
