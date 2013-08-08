#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([vo-aacenc], [0.1.1], [http://sourceforge.net/projects/opencore-amr/])
AC_CONFIG_AUX_DIR(.)
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([tar-ustar])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AC_CONFIG_HEADERS([config.h])
AM_MAINTAINER_MODE

# Cross compiling support
AC_CANONICAL_BUILD
AC_CANONICAL_HOST

# Various options for configure
AC_ARG_ENABLE([armv5e],
            [AS_HELP_STRING([--enable-armv5e],
               [enable ARMV5E assembler (default is no)])],
            [armv5e=$enableval], [armv5e=no])
AC_ARG_ENABLE([armv7neon],
            [AS_HELP_STRING([--enable-armv7neon],
               [enable ARMV7 neon assembler (default is no)])],
            [armv7neon=$enableval], [armv7neon=no])

AC_ARG_ENABLE([example],
            [AS_HELP_STRING([--enable-example],
                [enable example encoding program (default is no)])],
            [example=$enableval], [example=no])

# Automake conditionals to set
AM_CONDITIONAL(ARMV5E, test x$armv5e = xyes)
AM_CONDITIONAL(ARMV7NEON, test x$armv7neon = xyes)
AM_CONDITIONAL(EXAMPLE, test x$example = xyes)

# Checks for programs.
AM_PROG_AS
AC_PROG_CC_C99

# Setup for libtool
AC_PROG_LIBTOOL
AC_SUBST(LIBTOOL_DEPS)

# soname version to use
# goes by ‘current[:revision[:age]]’ with the soname ending up as
# current.age.revision.
VO_AACENC_VERSION=0:2:0
AC_SUBST(VO_AACENC_VERSION)

AC_CONFIG_FILES([Makefile
                 vo-aacenc.pc])
AC_OUTPUT
