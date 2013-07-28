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
 * SourceStream.h
 *
 *  Created on: Jul 7, 2013
 *      Author: aclarke
 */

#ifndef SOURCESTREAM_H_
#define SOURCESTREAM_H_

#include <io/humble/video/HumbleVideo.h>
#include <io/humble/video/ContainerStream.h>

namespace io {
namespace humble {
namespace video {

class Decoder;
/**
 * A {@link ContainerStream} class that was read from a {@link Source}.
 */
class VS_API_HUMBLEVIDEO SourceStream : public io::humble::video::ContainerStream
{
public:
  /**
   * For containers with Stream.Disposition.DISPOSITION_ATTACHED_PIC,
   * this returns a read-only copy of the packet containing the
   * picture (needs to be decoded separately).
   */
  virtual MediaPacket* getAttachedPic()=0;
  /**
   * Get the decoder that can decode the information in this source stream.
   */
  virtual Decoder* getDecoder()=0;

protected:
  SourceStream();
  virtual
  ~SourceStream();
};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* SOURCESTREAM_H_ */
