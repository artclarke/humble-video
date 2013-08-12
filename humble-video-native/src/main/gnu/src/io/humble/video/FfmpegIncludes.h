/*******************************************************************************
 * Copyright (c) 2013, Art Clarke.  All rights reserved.
 *  
 * This file is part of Humble-Video.
 *
 * Humble-Video is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Humble-Video is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Humble-Video.  If not, see <http://www.gnu.org/licenses/>.
 *******************************************************************************/

#ifndef HUMBLEFFMPEGINCLUDES_H_
#define HUMBLEFFMPEGINCLUDES_H_

extern "C"
{
// Hack here to get rid of deprecation compilation warnings
//#define attribute_deprecated

// WARNING: This is GCC specific and is to fix a build issue
// in FFmpeg where UINT64_C is not always defined.  The
// __WORDSIZE value is a GCC constant
#define __STDC_CONSTANT_MACROS 1
#include <stdint.h>
#ifndef UINT64_C
# if __WORDSIZE == 64
#  define UINT64_C(c)	c ## UL
# else
#  define UINT64_C(c)	c ## ULL
# endif
#endif


#include <libavutil/avutil.h>
#include <libavutil/common.h>
#include <libavutil/pixdesc.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libavutil/parseutils.h>

#include <libavcodec/avcodec.h>
#include <libavfilter/avfilter.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavformat/avformat.h>

#include <libswresample/swresample.h>
#include <libswscale/swscale.h>

}
#endif /*HUMBLEFFMPEGINCLUDES_H_*/
