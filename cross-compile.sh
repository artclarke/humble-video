#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

HOST=$( ${DIR}/mk/config.guess )

humble_configure()
{
  PREFIX="$1/$2/"
  BUILD=$2
  echo "Building ${DIR}/configure in ${PREFIX}"
  mkdir -p ./$cross_os
  mkdir -p "${PREFIX}"
  (cd ./${cross_os} && (${DIR}/configure --prefix="${PREFIX}" --build="${BUILD}" | tee configure.log) && (make | tee make.out) && make install)
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
      x86_64-gnu-linux5 \
      i686-gnu-linux6
      do
        humble_configure "${STAGE_DIR}" $cross_os
      done
  ;;
esac

