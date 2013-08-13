#ifdef VS_OS_WINDOWS
/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the w64 mingw-runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */

#include <stdio.h>

/*
 * There is a bug in current mingw-w64 builds that forces people to depend
 * on quadmath-0.dll for this one method. It is not fixed as of Aug 8, 2012
 * on the mingw-w64 packaged with Ubuntu 12.04. Once that is fixed, this
 * horrible hack should be taken out back and shot.
 */
void __mingw_raise_matherr (int typ, const char *name, double a1, double a2,
			    double rslt)
{
  fprintf(stderr, "_matherr(): %s in %s(%g, %g)  (retval=%g)\n", 
	  typ, name, a1, a2, rslt);
}

#else
// Let's have something to make the Gods of GCC warnings happy.
int VS_NotAFunctionLikelyToEverBeNecessary() {
  return -1;
}
#endif // VS_OS_WINDOWS
