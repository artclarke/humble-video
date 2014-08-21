#*******************************************************************************
# Copyright (c) 2014, Andrew "Art" Clarke.  All rights reserved.
#
# This file is part of Humble-Video.
#
# Humble-Video is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Humble-Video is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with Humble-Video.  If not, see <http://www.gnu.org/licenses/>.
#*******************************************************************************
#!/bin/bash

# take AR from env, or if not, a sensible default
AR=${AR:-ar}
# take pkg-config from env, or if not a sensible default.
PKG_CONFIG=${PKG_CONFIG:-pkg-config}
LIBDIR="$1"
shift;
export PKG_CONFIG_PATH="${PKG_CONFIG_PATH}:${LIBDIR}/pkgconfig"

# first get all the libraries
LIBS=$( ${PKG_CONFIG} --libs-only-l $@ )

OUTPUT=
for l in ${LIBS}; do
  # strip the -l
  f=$( echo $l | sed -e 's/-l//' )
  # check if this archive exists
  a="${LIBDIR}/lib${f}.a"
  # make whatever we print skip libtool
  OUTPUT+="-XCClinker "
  if [ -f "${a}" ]; then
    # link DIRECTLY against the archive if it exists (to short-circuit the shared lib)
    OUTPUT+="\"${a}\" "
  else
    # just link the library
    OUTPUT+="${l} "
  fi
done
echo "${OUTPUT}"

