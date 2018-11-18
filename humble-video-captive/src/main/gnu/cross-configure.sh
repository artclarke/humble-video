#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

HOST=$( ${DIR}/mk/config.guess )

humble_configure()
{
  BUILD=$2
  PREFIX_DEBUG="$1/humble-video-arch-$2-debug/target/native"
  if [ ! -e ./${cross_os}-debug/Makefile ]; then
    echo "Building ${DIR}/configure in ${PREFIX_DEBUG}"
    mkdir -p ./${cross_os}-debug
    mkdir -p "${PREFIX_DEBUG}"
    (cd ./${cross_os}-debug && (${DIR}/configure --enable-optimizations=no --prefix="${PREFIX_DEBUG}" --host="${BUILD}" ${HUMBLE_CONFIGURE} | tee configure.log))
  else
    echo "Makefile appears to be up-to-date in ${cross_os}-debug"
  fi
  PREFIX="$1/humble-video-arch-$2/target/native"
  if [ ! -e ./${cross_os}/Makefile ]; then
    echo "Building ${DIR}/configure in ${PREFIX}"
    mkdir -p ./$cross_os
    mkdir -p "${PREFIX}"
    (cd ./${cross_os} && (${DIR}/configure --enable-optimizations=yes --prefix="${PREFIX}" --host="${BUILD}" ${HUMBLE_CONFIGURE} | tee configure.log))
  else
    echo "Makefile appears to be up-to-date in ${cross_os}"
  fi
}

STAGE_DIR="${HUMBLE_HOME:-/tmp}"

case $HOST in
  *darwin*)
    # Darwin can only cross compile on Darwin, and now only 64-bit as of November 2018
    for cross_os in \
      x86_64-apple-darwin18
      do
        humble_configure "${STAGE_DIR}" $cross_os
      done
  ;;
  *)
  # Fill in these later.
    for cross_os in \
      x86_64-w64-mingw32 \
      i686-w64-mingw32 \
      i686-pc-linux-gnu6 \
      x86_64-pc-linux-gnu6 
      do
        humble_configure "${STAGE_DIR}" $cross_os
      done
  ;;
esac

