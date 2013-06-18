#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

m4_define([MLIB_MAJOR_VERSION], [2])
m4_define([MLIB_MINOR_VERSION], [2])
AC_PREREQ(2.63b)
AC_INIT([librtmp],[MLIB_MAJOR_VERSION.MLIB_MINOR_VERSION],[info@xuggle.com])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall -Werror no-dependencies nostdinc tar-pax 1.10.2 ])
AM_SILENT_RULES([yes])
AC_CANONICAL_BUILD
AC_CANONICAL_HOST

AC_CONFIG_SRCDIR([src/librtmp/rtmp.h])
AC_CONFIG_HEADERS([config.h])

LIB_MAJOR_VERSION=MLIB_MAJOR_VERSION
LIB_MINOR_VERSION=MLIB_MINOR_VERSION
VERSION=${LIB_MAJOR_VERSION}.${LIB_MINOR_VERSION}.xuggle
AC_SUBST([VERSION])
AC_SUBST([LIB_MAJOR_VERSION])
AC_SUBST([LIB_MINOR_VERSION])
AC_DEFINE(LIB_MAJOR_VERSION, [LIB_MAJOR_VERSION], [Major version of library])
AC_DEFINE(LIB_MINOR_VERSION, [LIB_MINOR_VERSION], [Minor version of library])
CRYPTO_REQ=libssl,libcrypto
AC_SUBST([CRYPTO_REQ])

# Checks for programs.
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

LT_PREREQ([2.2.7b])
LT_INIT([disable-fast-install win32-dll])

# Checks for libraries.
PKG_CHECK_MODULES([OPENSSL], [openssl >= 1 ])
AC_CHECK_HEADERS([windows.h])
AC_CHECK_HEADERS([mmsystem.h], [], [], [
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif
])
AC_MSG_CHECKING([for timeGetTime in -lwinmm])
RTMP_CACHED_LIBS="${LIBS}"
LIBS="-lwinmm ${LIBS}"
AC_TRY_LINK(
[
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif
#ifdef HAVE_MMSYSTEM_H
#include <mmsystem.h>
#endif
],
[ (void) timeGetTime(); ],
[ 
  AC_MSG_RESULT([yes])
],
[
 AC_MSG_RESULT([no])
 LIBS="${RTMP_CACHED_LIBS}"
]
)
AC_CHECK_LIB([pthread], [pthread_create])

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h limits.h netdb.h netinet/in.h stddef.h stdint.h stdlib.h string.h sys/param.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT8_T
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_HEADER_STDBOOL
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_FSEEKO
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_FUNC_REALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([gethostbyname inet_ntoa memchr memset select socket strcasecmp strchr strdup strerror strncasecmp strrchr strstr strtol])

PRIVATE_LIBS="${LIBS}"
AC_SUBST([PRIVATE_LIBS])

AC_CONFIG_FILES([Makefile
librtmp.pc:src/librtmp/librtmp.pc.in])

AC_OUTPUT
