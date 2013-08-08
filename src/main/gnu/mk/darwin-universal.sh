#!/bin/sh

# Copyright (c) 2012 by Xuggle Inc.
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
# LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
# OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
# It is REQUESTED BUT NOT REQUIRED if you use this library, that you let 
# us know by sending e-mail to info\@xuggle.com telling us briefly how you're
# using the library and what you like or don't like about it.

# This scripts takes two xuggler build install trees for different architectures,
# and creates a new directory with universal binaries (i.e. both architectures in them).
# This is the preferred way to ship for Darwin so that users don't have to worry about
# the bit-width of their JVM.

OUTPUTDIR="$1"
INPUTDIR1="$2"
INPUTDIR2="$3"

lipo_suction_file()
{
  LIPO_OUT=$1
  LIPO_IN1=$2
  LIPO_IN2=$3
  LIPO_OUT_DIR=$( dirname "${LIPO_OUT}" )
  mkdir -p "${LIPO_OUT_DIR}"
  echo lipo "${LIPO_IN1}" "${LIPO_IN2}" -output "${LIPO_OUT}" -create
  lipo "${LIPO_IN1}" "${LIPO_IN2}" -output "${LIPO_OUT}" -create
}

lipo_suction_dir()
{
  DIR_OUT=$1
  DIR_IN1=$2
  DIR_IN2=$3
  DIR_EXT=$4

  for path in "${DIR_IN1}"/*"${DIR_EXT}"; do
   file=$( basename "${path}" )
   lipo_suction_file "${DIR_OUT}/${file}" "${DIR_IN1}/${file}" "${DIR_IN2}/${file}"
  done
}

lipo_suction_dir "${OUTPUTDIR}/bin" "${INPUTDIR1}/bin" "${INPUTDIR2}/bin" ""
lipo_suction_dir "${OUTPUTDIR}/lib" "${INPUTDIR1}/lib" "${INPUTDIR2}/lib" ".dylib"
