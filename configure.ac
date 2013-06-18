#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

# IMPORTANT: IF YOU CHANGE THESE VERSION NUMBERS YOU MUST
# ALSO CHANGE ANY CORRESPONDING VERSION NUMBERS IN THE
# JAVA build.xml FILE
m4_define([VSM4_LIB_MAJOR_VERSION], [5])
m4_define([VSM4_LIB_MINOR_VERSION], [5])

AC_INIT([xuggle-ffmpeg],[VSM4_LIB_MAJOR_VERSION.VSM4_LIB_MINOR_VERSION],[info@xuggle.com])
AC_PREREQ([2.63])
AC_PREFIX_DEFAULT([${XUGGLE_HOME:-/usr/local}])
AM_INIT_AUTOMAKE([-Wall -Werror no-dependencies nostdinc tar-pax 1.10.2 ])
AM_MAINTAINER_MODE
AM_SILENT_RULES([yes])
LT_INIT([disable-static disable-fast-install win32-dll])
LT_PREREQ([2.2.6])

AC_CANONICAL_BUILD
AC_CANONICAL_HOST
# I hate building on Mac even more than windows; really; it's hard to explain my loathing
CFLAGS="${CFLAGS}"
CXXFLAGS="${CXXFLAGS}"
LDFLAGS="${LDFLAGS}"
ASFLAGS="${ASFLAGS}"
case $host_os in
  *darwin*)
  case $host_cpu in
     *x86_64*|*amd64*)
     CFLAGS="-arch x86_64 ${CFLAGS}"
     CXXFLAGS="-arch x86_64 ${CXXFLAGS}"
     LDFLAGS="-arch x86_64 ${LDFLAGS}"
     ;;
     *i?86*)
     CFLAGS="-arch i386 ${CFLAGS}"
     CXXFLAGS="-arch i386 ${CXXFLAGS}"
     LDFLAGS="-arch i386 ${LDFLAGS}"
     ;;
     *ppc64*|*powerpc64*)
     CFLAGS="-arch ppc64 ${CFLAGS}"
     CXXFLAGS="-arch ppc64 ${CXXFLAGS}"
     LDFLAGS="-arch ppc64 ${LDFLAGS}"
     ;;
     *ppc*|*powerpc*)
     CFLAGS="-arch ppc ${CFLAGS}"
     CXXFLAGS="-arch ppc ${CXXFLAGS}"
     LDFLAGS="-arch ppc ${LDFLAGS}"
     ;;
  esac
  ;;
esac
AC_SUBST([ASFLAGS])
# Checks for programs.
AC_LANG([C++])
AC_PROG_CC
AC_PROG_CXX
AM_PROG_AS

# check for cross compiling
AC_MSG_CHECKING([for cross-compiling])
if test "x$host_alias" != "x"; then
  VS_CROSS_COMPILE=1
  AC_MSG_RESULT([yes])
  AC_MSG_CHECKING([for separate cross-compiling toolchain])
  if $( echo "${CC}" | grep -q "^${host_alias}-" ) ; then
    VS_CROSS_PREFIX="${host_alias}-"
    AC_MSG_RESULT([yes: \"${VS_CROSS_PREFIX}\" ]) #"
  else
    VS_CROSS_PREFIX=""
    AC_MSG_RESULT([no])
  fi
else
  AC_MSG_RESULT([no])
fi 
AC_SUBST([VS_CROSS_PREFIX])
AC_SUBST([VS_CROSS_COMPILE])
AM_CONDITIONAL([VS_CROSS_COMPILE], [ test "x$host_alias" != "x" ])

LIB_MAJOR_VERSION=VSM4_LIB_MAJOR_VERSION
LIB_MINOR_VERSION=VSM4_LIB_MINOR_VERSION
AC_SUBST([LIB_MAJOR_VERSION])
AC_SUBST([LIB_MINOR_VERSION])
AC_DEFINE(VS_LIB_MAJOR_VERSION, [VSM4_LIB_MAJOR_VERSION], [Major version of library])
AC_DEFINE(VS_LIB_MINOR_VERSION, [VSM4_LIB_MINOR_VERSION], [Minor version of library])

VS_CPPFLAGS=
VS_CFLAGS=
VS_CXXFLAGS=
VS_LDFLAGS=

AC_ARG_ENABLE([optimizations],
              [AS_HELP_STRING([--enable-optimizations],
                              [build libraries and executables with compiler optimizations (default is yes)])],
                              [],
                              [enable_optimizations=yes])
AC_MSG_CHECKING([whether to enable compiler optimizations])
AC_MSG_RESULT([$enable_optimizations])

AM_CONDITIONAL([VS_DEBUG], [ test "x$enable_optimizations" = xno ])
if test "x$enable_optimizations" = xno; then
  VS_DEBUG=1
  AC_SUBST([VS_DEBUG])
fi

HOST_OS=$host
case $host in
  *cygwin*)
  HOST_TYPE=Windows
  ;;
  *mingw*)
  HOST_TYPE=Windows
  ;;
  *darwin*)
  HOST_TYPE=Mac
  ;;
  *)
  HOST_TYPE=Linux
  ;;
esac
HOST_CPU=$host_cpu
HOST_VENDOR=$host_vendor
# strip gnu from the OS_C variable if there
HOST_OS_C=$( echo $host_os | sed -e "s/-gnu//" )

AC_SUBST([HOST_TYPE])
AC_SUBST([HOST_OS])
AC_SUBST([HOST_CPU])
AC_SUBST([HOST_VENDOR])
AC_SUBST([HOST_OS_C])
AC_SUBST([HOST_SO_LIBPATH])

AC_MSG_CHECKING([whether we smell bunny poop])
AC_MSG_RESULT([no])

AC_SUBST([LIBTOOL_DEPS])

AC_PATH_PROG([YASM],[yasm])
AC_ARG_VAR(YASM,
          [Location of the 'yasm' program; if found, we will build with some cool assembly on x86/x86_64 systems.])
AM_CONDITIONAL(HAVE_YASM, [ test "x$YASM" != "x" ])

AM_CONDITIONAL(VS_CAPTIVE_ZLIB, [ test "${HOST_TYPE}" != "Mac" ])

AC_MSG_CHECKING([whether to compile FFmpeg with GPL V3 code])
AC_ARG_ENABLE([gpl],
              [AS_HELP_STRING([--enable-gpl],
                              [whether to enable GPL V3 dependent code (defaults to yes)])],
                              [],
                              [enable_gpl=yes])
VS_ENABLE_GPL=$enable_gpl
AC_MSG_RESULT([$enable_gpl])
if test "x$VS_ENABLE_GPL" = "xyes"; then
  AC_MSG_WARN([Compiling the GNU General Public License version of FFmpeg])
  AC_DEFINE(VS_ENABLE_GPL, 1, AC_PACKAGE_NAME [ will enable any code that depends on GPL code ])
else
  AC_MSG_WARN([ 

  vvvvvvvvvvvvvvvvvvvvv WARNING vvvvvvvvvvvvvvvvvvvvv

  FFmpeg in LGPL mode cannot use libx264 (H264 encoding),
  and several assembly optimizations

  ^^^^^^^^^^^^^^^^^^^^^ WARNING ^^^^^^^^^^^^^^^^^^^^^

  ])
fi
AC_SUBST(VS_ENABLE_GPL)
AM_CONDITIONAL(VS_ENABLE_GPL,
               [ test "x$VS_ENABLE_GPL" = "xyes" ])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_FILES([Makefile
                 Makefile.global
                 zlib/Makefile
                 zlib/incarcerate
                 zlib/install.sh
                 zlib/uninstall.sh
                 openssl/Makefile
                 openssl/incarcerate
                 openssl/install.sh
                 openssl/uninstall.sh
                 libvpx/Makefile
                 libvpx/incarcerate
                 libvpx/install.sh
                 libvpx/uninstall.sh
                 librtmp/Makefile
                 librtmp/incarcerate
                 librtmp/install.sh
                 librtmp/uninstall.sh
                 libvo-aacenc/Makefile
                 libvo-aacenc/incarcerate
                 libopencore-amr/Makefile
                 libopencore-amr/incarcerate
                 libogg/Makefile
                 libogg/incarcerate
                 libvorbis/Makefile
                 libvorbis/incarcerate
                 libtheora/Makefile
                 libtheora/incarcerate
                 libspeex/Makefile
                 libspeex/incarcerate
                 libx264/Makefile
                 libx264/incarcerate
                 libmp3lame/Makefile
                 libmp3lame/incarcerate
                 libtiff/Makefile
                 libtiff/incarcerate
                 spandsp/Makefile
                 spandsp/incarcerate
                 liboil/Makefile
                 liboil/incarcerate
                 ffmpeg/Makefile
                 ffmpeg/incarcerate
                 ffmpeg/install.sh
                 ffmpeg/uninstall.sh
                 ])
AC_OUTPUT
