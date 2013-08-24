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
 * MuxerStream.h
 *
 *  Created on: Aug 14, 2013
 *      Author: aclarke
 */

#ifndef MUXERSTREAM_H_
#define MUXERSTREAM_H_

#include <io/humble/video/ContainerStream.h>
#include <io/humble/video/Encoder.h>

namespace io {
namespace humble {
namespace video {

class Muxer;
class VS_API_HUMBLEVIDEO MuxerStream : public io::humble::video::ContainerStream
{
public:
  /**
   * Get the Encoder that this stream was created with.
   */
  virtual Encoder* getEncoder();

  /**
   * Get the Muxer that this stream belongs to.
   */
  virtual Muxer* getMuxer();
#ifndef SWIG
  static MuxerStream* make(Container*, int32_t);
#endif // SWIG
protected:
  MuxerStream(Container* container, int32_t index);
  virtual
  ~MuxerStream();
};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* MUXERSTREAM_H_ */
