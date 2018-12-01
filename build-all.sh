#!/bin/bash

# This script runs all the combinations of builds to get a full
# release ready. It looks gnarly but it's all about
# 1) make sure we build the "builder" docker container
# 2) executve maven with the right arguments for our matrix of dependencies

maven_args() {
    HOST=$1
    DEBUG=$2
    GPL=$3
    MEMCHECK=$4
    DIR=$HOST

    # We make the directory name, only adding suffixes if debug or non gpl
    if [ ! "x${GPL}" = "xyes" ]; then
        DIR="${DIR}-nogpl"
    fi
    if [ "x${DEBUG}" = "xyes" ]; then
        DIR="${DIR}-debug"
    fi

    if [ "x${MEMCHECK}" = "xyes" ]; then
        MEM=""
    else
        # this is non obvious, but 'make check' runs tests in native code and
        # 'make memcheck' runs with Valgrind memory checking. This hack tells
        # the build system to run "make ${MEM}check", which is normally blank
        # but when it's the string 'mem' Valgrind starts.
        MEM="mem"
    fi
    MVN_ARGS=""
    MVN_ARGS="-Dio.humble.build.memcheck=${mem} ${MVN_ARGS}"
    MVN_ARGS="-Dio.humble.build.include_gpl_dependencies=${GPL} ${MVN_ARGS}"
    MVN_ARGS="-Dio.humble.build.debug=${DEBUG} ${MVN_ARGS}"
    MVN_ARGS="-Dio.humble.build.host_os_directory=${DIR} ${MVN_ARGS}"
    MVN_ARGS="-Dio.humble.build.host_os_name=${HOST} ${MVN_ARGS}"
    echo ${MVN_ARGS}
}

# set this to echo to see all the commands but not execute them
DEBUG=
NATIVE_RUN=

# We use the cache directory for Maven and CC Cache to cache their
# content
mkdir -p $(pwd)/humble-video-cache/.m2
mkdir -p $(pwd)/humble-video-cache/.ccache
UBUNTU_1204_RUN="docker run --rm -it --name humble-video-docker \
    -v $(pwd)/humble-video-cache/.ccache:/root/.ccache \
    -v $(pwd)/humble-video-cache/.m2:/root/.m2 \
    -v $(pwd):/source \
    --cap-add=SYS_PTRACE --security-opt seccomp=unconfined \
    humble-video-docker:latest"

# build docker
${DEBUG} docker build -t humble-video-docker -f humble-video-docker/Dockerfile.ubuntu-12.04 humble-video-docker || exit $?

# now build our matrix of options

## Apple MacOS!!!

# release build of mac os x
${DEBUG} ${NATIVE_RUN}      mvn $(maven_args x86_64-apple-darwin18 no yes no) install || exit $?
# debug build of mac os x
${DEBUG} ${NATIVE_RUN}      mvn $(maven_args x86_64-apple-darwin18 yes yes no) install || exit $?

## LINUX!!!

# debug build of 64-bit linux
${DEBUG} ${UBUNTU_1204_RUN} mvn $(maven_args x86_64-pc-linux-gnu6 yes yes no) install || exit $?

# memcheck on debug build of 64-bit linux
${DEBUG} ${UBUNTU_1204_RUN} mvn $(maven_args x86_64-pc-linux-gnu6 yes yes yes) install || exit $?

# release build of 64-bit linux
${DEBUG} ${UBUNTU_1204_RUN} mvn $(maven_args x86_64-pc-linux-gnu6 no yes no) install || exit $?

# note -- right now our docker container can't really run a 32-bit JVM, so
# we skip these tests.

# release build of 32-bit linux
${DEBUG} ${UBUNTU_1204_RUN} mvn $(maven_args i686-pc-linux-gnu6 no yes no) -DskipTests install || exit $?
# debug build of 32-bit linux
${DEBUG} ${UBUNTU_1204_RUN} mvn $(maven_args i686-pc-linux-gnu6 yes yes no) -DskipTests install || exit $?

## WINDOWS!!!

# On Windows we cannot test, so skip the tests

# release build of 64-bit windows
${DEBUG} ${UBUNTU_1204_RUN} mvn $(maven_args x86_64-w64-mingw32 no yes no) -DskipTests install || exit $?
# debug build of 64-bit windows
${DEBUG} ${UBUNTU_1204_RUN} mvn $(maven_args x86_64-w64-mingw32 yes yes no) -DskipTests install || exit $?

# release build of 32-bit windows
${DEBUG} ${UBUNTU_1204_RUN} mvn $(maven_args i686-w64-mingw32 no yes no) -DskipTests install || exit $?
# debug build of 32-bit windows
${DEBUG} ${UBUNTU_1204_RUN} mvn $(maven_args i686-w64-mingw32 yes yes no) -DskipTests install || exit $?
