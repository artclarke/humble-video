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

  int32_t imgSize = av_image_fill_arrays(frame->data, frame->linesize, data,
      (enum AVPixelFormat) frame->format, frame->width, frame->height, 1);
  if (imgSize != bufSize) {
    VS_ASSERT(imgSize == bufSize, "these should always be equal");
    VS_THROW(HumbleRuntimeError("could not fill image with data"));
  }

  // now, set up the reference buffers
  frame->extended_data = frame->data;
  for (int32_t i = 0; i < AV_NUM_DATA_POINTERS; i++) {
    if (frame->data[i])
      frame->buf[i] = AVBufferSupport::wrapIBuffer(buffer, frame->data[i], frame->linesize[0]);
  }
  // now fill in the AVBufferRefs where we pass of to FFmpeg care
  // of our buffer. Be kind FFmpeg.  Be kind.
  RefPointer<PixelFormatDescriptor> desc = PixelFormat::getDescriptor((PixelFormat::Type)frame->format);

  if (!desc) {
    VS_THROW(HumbleRuntimeError("could not get format descriptor"));
  }
  if (desc->getFlag(PixelFormatDescriptor::PIX_FMT_FLAG_PAL) ||
      desc->getFlag(PixelFormatDescriptor::PIX_FMT_FLAG_PSEUDOPAL)) {
    av_buffer_unref(&frame->buf[1]);
    frame->buf[1] = AVBufferSupport::wrapIBuffer(IBuffer::make(retval.value(), 1024));
    if (!frame->buf[1]) {
      VS_THROW(HumbleRuntimeError("memory failure"));
    }

    frame->data[1] = frame->buf[1]->data;
  }

  // and we're done.
  return retval.get();
}

MediaPictureImpl*
MediaPictureImpl::make(MediaPictureImpl* src, bool copy) {
  RefPointer<MediaPictureImpl> retval;

  if (!src) VS_THROW(HumbleInvalidArgument("no src object to copy from"));

  if (copy) {
    // first create a new mediaaudio object to copy into
    retval = make(src->getWidth(), src->getHeight(), src->getFormat());
    retval->mComplete = src->mComplete;

    // then copy the data into retval
    int32_t n = src->getNumDataPlanes();
    for(int32_t i = 0; i < n; i++ )
    {
      AVBufferRef* dstBuf = av_frame_get_plane_buffer(retval->mFrame, i);
      AVBufferRef* srcBuf = av_frame_get_plane_buffer(src->mFrame, i);
      VS_ASSERT(dstBuf, "should always have buffer");
      VS_ASSERT(srcBuf, "should always have buffer");
      memcpy(dstBuf->data, srcBuf->data, srcBuf->size);
    }
  } else {
    // first create a new media audio object to reference into
    retval = make();

    // then do the reference
    retval->mComplete = src->mComplete;
    av_frame_ref(retval->mFrame, src->mFrame);
  }
  return retval.get();}

void
MediaPictureImpl::setComplete(bool val, int64_t timestamp) {
  mComplete = val;
  mFrame->pts = timestamp;
}

void
MediaPictureImpl::validatePlane(int32_t plane)
{
  if (plane < 0) {
    VS_THROW(HumbleInvalidArgument("plane must be >= 0"));
  }

  if (plane >= 4) {
    VS_THROW(HumbleInvalidArgument("plane must be < getNumDataPlanes()"));
  }

}
IBuffer*
MediaPictureImpl::getData(int32_t plane) {
  validatePlane(plane);
  // we get the buffer for the given plane if it exists, and wrap
  // it in an IBuffer
  // now we're guaranteed that we should have a plane.
  RefPointer<IBuffer> buffer;
  if (mFrame->buf[plane])
    buffer = AVBufferSupport::wrapAVBuffer(this,
        mFrame->buf[plane], mFrame->data[plane], mFrame->linesize[0]);
  return buffer.get();}

int32_t
MediaPictureImpl::getDataPlaneSize(int32_t plane) {
  validatePlane(plane);
  return mFrame->buf[plane] ? mFrame->buf[plane]->size : 0;
}
int32_t
MediaPictureImpl::getLineSize(int32_t plane) {
  validatePlane(plane);
  return mFrame->linesize[plane];
}

int64_t
MediaPictureImpl::getError(int32_t plane) {
  validatePlane(plane);
  return mFrame->error[plane];
}

int32_t
MediaPictureImpl::getNumDataPlanes() {
  int32_t i = 0;
  for(; i < AV_NUM_DATA_POINTERS; i++)
    if (!mFrame->data[i])
      break;
  return i == 0 ? 0 : i-1;
}

} /* namespace video */
} /* namespace humble */
} /* namespace io */
