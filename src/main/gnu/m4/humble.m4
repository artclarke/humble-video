# LICENSE
#  Copyright (C) 2012, Art Clarke
#  Permission is hereby granted, free of charge, to any person obtaining
#  a copy of this software and associated documentation files (the
#  "Software"), to deal in the Software without restriction, including
#  without limitation the rights to use, copy, modify, merge, publish,
#  distribute, sublicense, and/or sell copies of the Software, and to
#  permit persons to whom the Software is furnished to do so, subject to
#  the following conditions:
#
#  The above copyright notice and this permission notice shall be
#  included in all copies or substantial portions of the Software.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
#  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
#  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
#  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
#  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
#  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
#  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#  It is REQUESTED BUT NOT REQUIRED if you use this library, that you let 
#  us know by sending e-mail to info\@xuggle.com telling us briefly how you're
#  using the library and what you like or don't like about it.
#
#  Thank you.
#
# SYNOPSIS
#

#   VS_FIND_STATIC_LIB
#
# DESCRIPTION
#   Xuggle builds a shared object that will be loaded into a Java Virtual Machine,
#   but is also build with C++.  When using GCC it is important to link to STATIC
#   versions of the GCC and STDC++ libraries if available to avoid a run-time
#   dependency on machines that don't have the dynamic version of those libraries.
#
#   This macro attempts to find a static position-independent (PIC) version of
#   a library, and tests linking PIC code directly against that file.  If it
#   can do this, it sets VS_LIB to the value of the file.
#
#   VS_FIND_STATIC_LIB( library, ON-SUCCESS, ON-FAIL )
#
#
AC_DEFUN([VS_FIND_STATIC_LIB], [
  VS_LIB=
  AC_LANG_SAVE
  AC_LANG_C
  vs_save_CFLAGS="$CFLAGS"
  vs_save_LDFALGS="$LDFLAGS"
  CFLAGS="$CFLAGS -fno-exceptions -fPIC -DPIC"
  VS_LIB_CANDIDATE="$1"
  AC_MSG_CHECKING([for static PIC version of ${VS_LIB_CANDIDATE} ])
  vs_cv_find_static_lib=no

  vs_LDFLAGS="$LDFLAGS -Wl,--no-undefined -Wl,--no-allow-shlib-undefined -shared -static-libgcc -lgcc_eh -lgcc -lc"
  VS_LIB_FILE=`${CC} --print-file-name="lib${VS_LIB_CANDIDATE}.a"`
  AS_IF([ test -f "${VS_LIB_FILE}" ], [
    VS_LIB="${VS_LIB_FILE}"
    LDFLAGS="-Wl,--whole-archive ${VS_LIB} -Wl,--no-whole-archive ${vs_LDFLAGS}"
    AC_LINK_IFELSE([AC_LANG_PROGRAM(
        [],
        [ int i = 1; ]
    )],
        [vs_cv_find_static_lib=yes],
        [vs_cv_find_static_lib=no])
  ], [])
  AS_IF([ test $vs_cv_find_static_lib = no ], [
    VS_LIB_FILE=`${CC} --print-file-name="lib${VS_LIB_CANDIDATE}_pic.a"`
    AS_IF([ test -f "${VS_LIB_FILE}" ], [
      VS_LIB="${VS_LIB_FILE}"
      LDFLAGS="-Wl,--whole-archive ${VS_LIB} -Wl,--no-whole-archive ${vs_LDFLAGS}"
      AC_LINK_IFELSE([AC_LANG_PROGRAM(
          [],
          [ int i = 1; ]
      )],
          [vs_cv_find_static_lib=yes],
          [vs_cv_find_static_lib=no])
    ], [])
  ], [])
  AS_IF([ test $vs_cv_find_static_lib = yes ], [
    AC_MSG_RESULT(yes : ${VS_LIB})
    $2
  ], [
    AC_MSG_RESULT(no)
    $3
  ])
  CFLAGS="$vs_save_CFLAGS"
  LDFLAGS="$vs_save_LDFLAGS"
  AC_LANG_RESTORE
])

