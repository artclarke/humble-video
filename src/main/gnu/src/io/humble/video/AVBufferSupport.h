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
 * AVBufferSupport.h
 *
 *  Created on: Jul 12, 2013
 *      Author: aclarke
 */

#ifndef AVBUFFERSUPPORT_H_
#define AVBUFFERSUPPORT_H_

#include <io/humble/video/HumbleVideo.h>
#include <io/humble/ferry/RefCounted.h>
#include <io/humble/ferry/Buffer.h>
namespace io {
namespace humble {
namespace video {

class AVBufferSupport
{
public:
  /**
   * Wraps a AVBufferRef in an Buffer.
   */
  static io::humble::ferry::Buffer* wrapAVBuffer(io::humble::ferry::RefCounted* requestor, AVBufferRef* ref);
  static io::humble::ferry::Buffer* wrapAVBuffer(io::humble::ferry::RefCounted* requestor, AVBufferRef* ref,
      void* data, int32_t size);
  /**
   * Wraps an AVBufferRef in a Buffer
   */
  static AVBufferRef* wrapBuffer(io::humble::ferry::Buffer* buf);
  static AVBufferRef* wrapBuffer(io::humble::ferry::Buffer* buf, void *data, int32_t size);

private:
  static void bufferRelease(void * closure, uint8_t * buf);
  static void avBufferRelease(void * buf, void * closure);

  AVBufferSupport();
  virtual
  ~AVBufferSupport();
};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* AVBUFFERSUPPORT_H_ */
