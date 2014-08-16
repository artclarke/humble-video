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

#ifndef __HUMBLEVIDEO_H__
#define __HUMBLEVIDEO_H__

#include <io/humble/Humble.h>
#include <io/humble/video/FfmpegIncludes.h>
#include <inttypes.h>

namespace io {
namespace humble {
/**
 * This package contains the core Humble Video library routines
 * that deal with the manipulation of media files.
 *
 * To get started, check out Global.h and the Global object.
 */
namespace video {
}
}
}
#ifndef VS_API_HUMBLEVIDEO
#define VS_API_HUMBLEVIDEO VS_API_EXPORT
#endif // ! VS_API_HUMBLEVIDEO

#endif // ! __HUMBLEVIDEO_H__
