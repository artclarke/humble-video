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
 * AVBufferSupport.cpp
 *
 *  Created on: Jul 12, 2013
 *      Author: aclarke
 */

#include "AVBufferSupport.h"

using namespace io::humble::ferry;

namespace io {
namespace humble {
namespace video {

AVBufferSupport::AVBufferSupport() {
}

AVBufferSupport::~AVBufferSupport() {
}

/**
 * Wraps a AVBufferRef in an IBuffer.
 */
IBuffer*
AVBufferSupport::wrapAVBuffer(RefCounted* requestor, AVBufferRef* ref) {
  // acquire for the IBuffer release method.
  AVBufferRef* ourRef = av_buffer_ref(ref);
  if (!ourRef) return 0;

  int32_t size = ourRef->size;
  uint8_t* data = ourRef->data;

  IBuffer* retval = IBuffer::make(requestor, data, size,
      AVBufferSupport::avBufferRelease, ourRef);
  if (!retval) av_buffer_unref(&ourRef);
  return retval;
}
/**
 * Wraps an AVBufferRef in a IBuffer
 */
AVBufferRef*
AVBufferSupport::wrapIBuffer(IBuffer* buf) {
  if (!buf) return 0;
  int32_t size = buf->getBufferSize();
  uint8_t* data = (uint8_t*) buf->getBytes(0, size);
  // acquire the buffer for the AVBufferRef
  buf->acquire();
  AVBufferRef* retval = av_buffer_create(data, size,
      AVBufferSupport::bufferRelease, buf, 0);
  if (!retval) buf->release();
  return retval;
}

void
AVBufferSupport::bufferRelease(void * closure, uint8_t * buf) {
  IBuffer* b = (IBuffer*) closure;
  (void) buf;
  if (b) b->release();
}
void
AVBufferSupport::avBufferRelease(void * buf, void * closure) {
  AVBufferRef *b = (AVBufferRef*) closure;
  (void) buf;
  av_buffer_unref(&b);
}

} /* namespace video */
} /* namespace humble */
} /* namespace io */
