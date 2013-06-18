/*****************************************************************************
 * osdep.c: platform-specific code
 *****************************************************************************
 * Copyright (C) 2003-2013 x264 project
 *
 * Authors: Steven Walters <kemuri9@gmail.com>
 *          Laurent Aimar <fenrir@via.ecp.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111, USA.
 *
 * This program is also available under a commercial proprietary license.
 * For more information, contact us at licensing@x264.com.
 *****************************************************************************/

#include "common.h"

#if SYS_WINDOWS
#include <sys/types.h>
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif
#include <time.h>

#if PTW32_STATIC_LIB
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
/* this is a global in pthread-win32 to indicate if it has been initialized or not */
extern int ptw32_processInitialized;
#endif

int64_t x264_mdate( void )
{
#if SYS_WINDOWS
    struct timeb tb;
    ftime( &tb );
    return ((int64_t)tb.time * 1000 + (int64_t)tb.millitm) * 1000;
#else
    struct timeval tv_date;
    gettimeofday( &tv_date, NULL );
    return (int64_t)tv_date.tv_sec * 1000000 + (int64_t)tv_date.tv_usec;
#endif
}

#if HAVE_WIN32THREAD || PTW32_STATIC_LIB
/* state of the threading library being initialized */
static volatile LONG x264_threading_is_init = 0;

static void x264_threading_destroy( void )
{
#if PTW32_STATIC_LIB
    pthread_win32_thread_detach_np();
    pthread_win32_process_detach_np();
#else
    x264_win32_threading_destroy();
#endif
}

int x264_threading_init( void )
{
    /* if already init, then do nothing */
    if( InterlockedCompareExchange( &x264_threading_is_init, 1, 0 ) )
        return 0;
#if PTW32_STATIC_LIB
    /* if static pthread-win32 is already initialized, then do nothing */
    if( ptw32_processInitialized )
        return 0;
    if( !pthread_win32_process_attach_np() )
        return -1;
#else
    if( x264_win32_threading_init() )
        return -1;
#endif
    /* register cleanup to run at process termination */
    atexit( x264_threading_destroy );

    return 0;
}
#endif

#if HAVE_MMX
#ifdef __INTEL_COMPILER
/* Agner's patch to Intel's CPU dispatcher from pages 131-132 of
 * http://agner.org/optimize/optimizing_cpp.pdf (2011-01-30)
 * adapted to x264's cpu schema. */

// Global variable indicating cpu
int __intel_cpu_indicator = 0;
// CPU dispatcher function
void x264_intel_cpu_indicator_init( void )
{
    unsigned int cpu = x264_cpu_detect();
    if( cpu&X264_CPU_AVX )
        __intel_cpu_indicator = 0x20000;
    else if( cpu&X264_CPU_SSE42 )
        __intel_cpu_indicator = 0x8000;
    else if( cpu&X264_CPU_SSE4 )
        __intel_cpu_indicator = 0x2000;
    else if( cpu&X264_CPU_SSSE3 )
        __intel_cpu_indicator = 0x1000;
    else if( cpu&X264_CPU_SSE3 )
        __intel_cpu_indicator = 0x800;
    else if( cpu&X264_CPU_SSE2 && !(cpu&X264_CPU_SSE2_IS_SLOW) )
        __intel_cpu_indicator = 0x200;
    else if( cpu&X264_CPU_SSE )
        __intel_cpu_indicator = 0x80;
    else if( cpu&X264_CPU_MMX2 )
        __intel_cpu_indicator = 8;
    else
        __intel_cpu_indicator = 1;
}

/* __intel_cpu_indicator_init appears to have a non-standard calling convention that
 * assumes certain registers aren't preserved, so we'll route it through a function
 * that backs up all the registers. */
void __intel_cpu_indicator_init( void )
{
    x264_safe_intel_cpu_indicator_init();
}
#else
void x264_intel_cpu_indicator_init( void )
{}
#endif
#endif
