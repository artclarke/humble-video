#!/bin/bash

# take AR from env, or if not, a sensible default
AR=${AR:-ar}
LIBDIR="$1"
shift;
export PKG_CONFIG_PATH="${PKG_CONFIG_PATH}:${LIBDIR}/pkgconfig"

# first get all the libraries
LIBS=$( pkg-config --libs-only-l $@ )

for l in ${LIBS}; do
  # strip the -l
  f=$( echo $l | sed -e 's/-l//' )
  # check if this archive exists
  a="${LIBDIR}/lib${f}.a"
  # make whatever we print skip libtool
  echo -n "-XCClinker "
  if [ -f "${a}" ]; then
    # link DIRECTLY against the archive if it exists (to short-circuit the shared lib)
    echo -n "\"${a}\" "
  else
    # just link the library
    echo -n "${l} "
  fi
done
echo

