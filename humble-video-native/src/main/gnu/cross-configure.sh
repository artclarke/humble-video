#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

HOST=$( ${DIR}/mk/config.guess )

humble_configure()
{
  PREFIX="$1/humble-video-arch-$2/target/native"
  BUILD=$2
  if [ ! -e ./${cross_os}/Makefile ]; then
    echo "Building ${DIR}/configure in ${PREFIX}"
    mkdir -p ./$cross_os
    mkdir -p "${PREFIX}"
    (cd ./${cross_os} && (${DIR}/configure --prefix="${PREFIX}" --host="${BUILD}" ${HUMBLE_CONFIGURE} | tee configure.log))
  else
    echo "Makefile appears to be up-to-date in ${cross_os}"
  fi
}

STAGE_DIR="${HUMBLE_HOME:-/tmp}"

case $HOST in
  *darwin*)
    # Darwin can only cross compile on Darwin
    for cross_os in \
      x86_64-apple-darwin12 \
      i686-apple-darwin12
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

