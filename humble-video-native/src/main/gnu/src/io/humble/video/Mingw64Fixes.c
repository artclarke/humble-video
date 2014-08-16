#ifdef VS_OS_WINDOWS
/*******************************************************************************
 * Copyright (c) 2014, Andrew "Art" Clarke.  All rights reserved.
 *   
 * This file is part of Humble-Video.
 *
 * Humble-Video is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Humble-Video is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Humble-Video.  If not, see <http://www.gnu.org/licenses/>.
 *******************************************************************************/

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
