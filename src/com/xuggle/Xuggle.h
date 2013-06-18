/*******************************************************************************
 * Copyright (c) 2008, 2010 Xuggle Inc.  All rights reserved.
 *  
 * This file is part of Xuggle-Xuggler-Main.
 *
 * Xuggle-Xuggler-Main is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Xuggle-Xuggler-Main is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Xuggle-Xuggler-Main.  If not, see <http://www.gnu.org/licenses/>.
 *******************************************************************************/

#ifndef XUGGLE_H_
#define XUGGLE_H_

#ifdef __cplusplus
/**
 * This package is empty and is just a containing namespace
 * for xuggle
 */
namespace com
{

/**
 * This package contains Native code belonging to
 * Xuggle libraries.
 */
namespace xuggle {

}
}

#endif // __cplusplus

#define VS_STRINGIFY(__arg) #__arg

#if (__GNUC__ >= 4) || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
#  ifndef GCC_HASCLASSVISIBILITY
#    define GCC_HASCLASSVISIBILITY
#  endif
#endif

#ifndef VS_API_EXPORT
# if defined(_WIN32) || defined(__WIN32__) || defined(__CYGWIN__) || defined(VS_OS_WINDOWS)
#   ifdef VS_API_COMPILING
#     define VS_API_EXPORT __declspec(dllexport)
#   else
#     define VS_API_EXPORT __declspec(dllimport)
#   endif
#   define VS_API_CALL __stdcall
# else
#   if defined(__GNUC__) && defined(GCC_HASCLASSVISIBILITY)
#     if VS_API_COMPILING
#       define VS_API_EXPORT __attribute__ ((visibility("default")))
#     else
#       define VS_API_EXPORT
#     endif
#   else
#     define VS_API_EXPORT
#   endif
#   define VS_API_CALL
# endif
#endif

#ifdef JNIEXPORT
#undef JNIEXPORT
#define JNIEXPORT "_____ERROR___USE_VS_API_EXPORT_INSTEAD_OF_JNIEXPORT____________"
#endif

#ifdef JNICALL
#undef JNICALL
#define JNICALL VS_API_CALL
#endif

#endif // XUGGLE_H_
