#!/bin/bash

# assumes the following command line arguments

CONFIGURE=$1 # The absolute path of the configure script to run
PREFIX=$2 # The absolute path we will install into.
HOST=$3   # The cross compilation host string
DEBUG=$4  # A 'yes' or 'no' flag for if we build debug versions or not
GPL=$5    # A 'yes' or 'no' flag for if we build debug versions or not.

# Convert the prefix into an absolute path because configure needs that
ABS_PREFIX="$(cd "$(dirname "$PREFIX")"; pwd)/$(basename "$PREFIX")"

CONFIGURE_FLAGS=
CONFIGURE_FLAGS="--prefix=${ABS_PREFIX} ${CONFIGURE_FLAGS}"
CONFIGURE_FLAGS="--host=${HOST} ${CONFIGURE_FLAGS}"

if [ "${GPL}" = "yes" ]; then
    CONFIGURE_FLAGS="--enable-gpl=yes ${CONFIGURE_FLAGS}"
else
    CONFIGURE_FLAGS="--enable-gpl=no ${CONFIGURE_FLAGS}"
fi
if [ "${DEBUG}" = "yes" ]; then
    CONFIGURE_FLAGS="--enable-optimizations=no ${CONFIGURE_FLAGS}"
else
    CONFIGURE_FLAGS="--enable-optimizations=yes ${CONFIGURE_FLAGS}"
fi

echo ${CONFIGURE} ${CONFIGURE_FLAGS}
if [ -e Makefile ]; then
    echo "Makefile appears to be up-to-date in $(pwd). Skipping configure."
else
    ${CONFIGURE} ${CONFIGURE_FLAGS}
fi
