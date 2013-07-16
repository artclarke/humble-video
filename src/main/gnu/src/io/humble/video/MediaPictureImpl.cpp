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
 * MediaPictureImpl.cpp
 *
 *  Created on: Jul 13, 2013
 *      Author: aclarke
 */

#include "MediaPictureImpl.h"

#include "AVBufferSupport.h"
#include <io/humble/ferry/Logger.h>
#include <io/humble/ferry/HumbleException.h>
#include <io/humble/ferry/RefPointer.h>

using namespace io::humble::ferry;

VS_LOG_SETUP(VS_CPP_PACKAGE);

namespace io {
namespace humble {
namespace video {

MediaPictureImpl::MediaPictureImpl() :
    mFrame(0) {
  mFrame = av_frame_alloc();
  if (!mFrame) throw std::bad_alloc();
  mFrame->opaque = this;
  mComplete = false;
}

MediaPictureImpl::~MediaPictureImpl() {
  av_frame_free(&mFrame);
}

MediaPictureImpl*
MediaPictureImpl::make(int32_t width, int32_t height,
    PixelFormat::Type format) {
  if (width <= 0)
  VS_THROW(HumbleInvalidArgument("width must be > 0"));

  if (height <= 0)
  VS_THROW(HumbleInvalidArgument("height must be > 0"));

  if (format == PixelFormat::PIX_FMT_NONE)
  VS_THROW(HumbleInvalidArgument("pixel format must be specifie"));

  // let's figure out how big of a buffer we need
  int32_t bufSize = avpicture_get_size((enum AVPixelFormat) format, width,
      height);

  RefPointer<IBuffer> buffer = IBuffer::make(0, bufSize);
  MediaPictureImpl* retval = make(buffer.value(), width, height, format);
  if (retval) buffer->setJavaAllocator(retval->getJavaAllocator());

  return retval;
}

MediaPictureImpl*
MediaPictureImpl::make(IBuffer* buffer, int32_t width, int32_t height,
    PixelFormat::Type format) {
  if (width <= 0) {
    VS_THROW(HumbleInvalidArgument("width must be > 0"));
  }

  if (height <= 0) {
    VS_THROW(HumbleInvalidArgument("height must be > 0"));
  }

  if (format == PixelFormat::PIX_FMT_NONE) {
    VS_THROW(HumbleInvalidArgument("pixel format must be specifie"));
  }

  if (!buffer) {
    VS_THROW(HumbleInvalidArgument("must pass non null buffer"));
  }

  // let's figure out how big of a buffer we need
  int32_t bufSize = av_image_get_buffer_size((enum AVPixelFormat) format, width,
      height, 1);
  if (bufSize < buffer->getBufferSize()) {
    VS_THROW(
        HumbleInvalidArgument(
            "passed in buffer too small to fit requested image parameters"));
  }

  RefPointer<MediaPictureImpl> retval = make();
  AVFrame* frame = retval->mFrame;
  frame->width = width;
  frame->height = height;
  frame->format = format;

  // buffer is large enough; let's fill the data pointers
  uint8_t* data = (uint8_t*) buffer->getBytes(0, bufSize);

  int32_t imgSize = av_image_fill_arrays(frame->data, frame->linesize,
      data,
      (enum AVPixelFormat) frame->format,
      frame->width,
      frame->height,
      1);
  if (imgSize != bufSize) {
    VS_ASSERT(imgSize == bufSize, "these should always be equal");
    VS_THROW(HumbleRuntimeError("could not fill image with data"));
  }

  // now fill in the AVBufferRefs where we pass of to FFmpeg care
  // of our buffer. Be kind FFmpeg.  Be kind.

  // and we're done.
  return retval.get();
}

MediaPictureImpl*
MediaPictureImpl::make(MediaPictureImpl* src, bool copy) {
  // TODO: Implement
  (void) src;
  (void) copy;
  return 0;
}

void
MediaPictureImpl::setComplete(bool val, int64_t timestamp) {
  mComplete = val;
  mFrame->pts = timestamp;
}

IBuffer*
MediaPictureImpl::getData(int32_t plane) {
  (void) plane;
  return 0;
}

int32_t
MediaPictureImpl::getDataPlaneSize() {
  return 0;
}

int64_t
MediaPictureImpl::getError(int32_t plane) {
  if (plane < 0 || plane >= 4)
  VS_THROW(HumbleInvalidArgument("plane must be between 0 and 3"));
  return mFrame->error[plane];
}

int32_t
MediaPictureImpl::getNumDataPlanes() {
  return 0;
}

} /* namespace video */
} /* namespace humble */
} /* namespace io */
