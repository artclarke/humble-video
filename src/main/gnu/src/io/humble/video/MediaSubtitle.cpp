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
/*
 * MediaSubtitle.cpp
 *
 *  Created on: Jul 23, 2013
 *      Author: aclarke
 */

#include "MediaSubtitle.h"
#include <io/humble/ferry/RefPointer.h>

using namespace io::humble::ferry;

namespace io {
namespace humble {
namespace video {

MediaSubtitle::MediaSubtitle() : mCtx(0) {

}

MediaSubtitle::~MediaSubtitle() {
}

MediaSubtitle*
MediaSubtitle::make(AVSubtitle* ctx)
{
  if (!ctx)
    throw HumbleInvalidArgument("no context");
  RefPointer<MediaSubtitle> retval = MediaSubtitle::make();
  retval->mCtx = ctx;
  return retval.get();
}

MediaSubtitleRectangle*
MediaSubtitle::getRectangle(int32_t n)
{
  if (n < 0 || n >= (int32_t)mCtx->num_rects)
    throw HumbleInvalidArgument("attempt to get out-of-range rectangle");

  if (!mCtx->rects)
    throw HumbleRuntimeError("no rectangles");

  return MediaSubtitleRectangle::make(mCtx->rects[n]);
}

MediaSubtitleRectangle*
MediaSubtitleRectangle::make(AVSubtitleRect* ctx) {
  if (!ctx)
    throw HumbleInvalidArgument("no context");
  RefPointer<MediaSubtitleRectangle> retval = make();
  retval->mCtx = ctx;
  return retval.get();
}
int32_t
MediaSubtitleRectangle::getPictureLinesize(int line) {
  if (line < 0 || line >= 4)
    throw HumbleInvalidArgument("line must be between 0 and 3");
  return mCtx->pict.linesize[line];
}
IBuffer*
MediaSubtitleRectangle::getPictureData(int line)
{
  if (line < 0 || line >= 4)
    throw HumbleInvalidArgument("line must be between 0 and 3");
  // add ref ourselves for the IBuffer
  this->acquire();
  // create a buffer
  RefPointer<IBuffer> retval = IBuffer::make(this, mCtx->pict.data[line], mCtx->pict.linesize[line],
      IBuffer::refCountedFreeFunc, this);
  if (!retval)
    this->release();
  return retval.get();
}

} /* namespace video */
} /* namespace humble */
} /* namespace io */
