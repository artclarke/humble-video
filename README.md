# Please read this BEFORE running 'mvn install'.

Welcome to the Humble Video uber project.

HumbleVideo allows Java Virtual Machine developers (Java, Scala, Clojure, JRuby, etc.) to decode, analyze/modify
and encode audio and video data into 100s of different formats (e.g. H264, AAC, MP3, FLV, etc.). It uses the
<a href="http://ffmpeg.org">FFmpeg</a> project under the covers.

Humble Video is a mix of Java and native code, and the native code is written in C++, C and Assembly. Most users
are **strongly** encouraged to use the maven artifacts in a central repository, as building the system is complicated.
It requires accesss to a Mac OS X development environment (Mountain Lion or later).

It consists of several sub-projects which are detailed below:

* `humble-video-parent/`: The parent pom to all projects, where global defaults (like version numbers) are set.
* `humble-video-chef/`: A collection of chef-solo recipes for configuring a Vagrant Ubuntu-64-bit box to build Linux and Windows versions.
* `humble-video-captive/`: All open-source libraries incorporated into humble-video. These are built with a custom build system.
* `humble-video-native/`: All native (C/C++/Assembly) code for Humble-Video. These are built with a custom build system that generates
     many Maven artifacts containing native code. and generate Java files. The maven artifacts generated are of the form:
     `humble-video-arch-${architecture-specifier}.jar` and the architecture-specifier looks like `i686-gnu-linux` for 32-bit linux, etc.
     For users who do not want to always download artifacts that contain all architectures, these are valid POMs to be depended on.
  that are inserted into `humble-video-all`.
* `humble-video-noarch/`: All Java code (and only Java code), including some generated Java code from `humble-video-native`. Unit tests run from here.  These are built with maven. 
* `humble-video-test/`: All integration (very long running) tests for humble-video. Depends on humble-video-noarch and (via maven profiles) whatever architecture specific versions of the native code Jars it needs.
* `humble-video-all/`: A pom that depends on humble-video-noarch and humble-video-arch-all. This will only successfully build on build machines that have all architecture files required staged (i.e. probably not your box).
* `humble-video-stage/`: Where all the native code's final artifacts (DLLs and the like) end up being place so that all the rest of the maven build system knows where to find them

To build, assuming you have all the prerequisites installed for your OS (see below), run:

    mvn install

To clean out a build tree, you need to do:

    (cd humble-video-stage && mvn clean) && mvn clean

That will clean out all the staged binaries in addition to the intermediate files built for each operating system.

# Pre-requisites on build machines

This project builds native code that can be used for Humble Software, and
also generates Java files that can be compiled into Maven libraries
that use the native code.

To build the native code the autotools stack is required:

* GNU c++ compilers
* YASM (for assembly)
* pkg-config
* The Humble-Swig version of Swig (Use https://github.com/artclarke/humble-swig to get JavaDoc generated).
* Doxygen & DOT (for C++ documentation)
* Valgrind (for auto memory leak detection -- try 'make memcheck')
* Java JDK 1.6 or higher

## Mac OS X Notes

Mac OS X is the preferred environment for building Humble Video because it allows you
to build all supported environments (Mac, Linux and Darwin) if you install Oracle's
VirtualBox software and the excellent [Vagrant](http://vagrantup.com) program.

You must install the JDK on your mac, and also Apple's xCode developer tools.

The 'brew' utility is your on Mac OS X friend. Use it often. You can install yasm using that.

    brew install pkg-config
    brew install yasm
    brew install valgrind

Also, on Mac OS X the Valgrind test suite only works for 64-bit builds.

To cross-compile for linux and windows in addition to Mac OS X, run:

    # On your Mac OS X terminal
    mvn install
    vagrant up

    # Once your vagrant box is provisioned
    vagrant ssh

    # From the Vagrant shell
    cd /vagrant
    mvn install

## Linux Build Notes

You need to make sure the Chef recipes that are in `./Vagrantfile` get installed on your Linux box. Those recipes are in the humble-video-chef project. 

I've only tested on Ubuntu 12.04 LTS, so that's all that supported to build on, and it's a work in progress so hang tight..

To build, once the chef recipes have run on a clean box, just run:

    mvn install

## Windows Build Notes
Don't do it. It's not supported. Instead you have to cross-compile for Windows from a Linux box. Sorry.

Enjoy!

- Art
