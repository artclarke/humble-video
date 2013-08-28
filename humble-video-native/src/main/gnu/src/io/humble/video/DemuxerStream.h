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
 * DemuxerStream.h
 *
 *  Created on: Jul 7, 2013
 *      Author: aclarke
 */

#ifndef DEMUXERSTREAM_H_
#define DEMUXERSTREAM_H_

#include <io/humble/video/HumbleVideo.h>
#include <io/humble/video/ContainerStream.h>
#include <io/humble/video/Decoder.h>

namespace io {
namespace humble {
namespace video {

class Demuxer;
/**
 * A ContainerStream object representing a stream contained in a readable Container.
 */
class VS_API_HUMBLEVIDEO DemuxerStream : public io::humble::video::ContainerStream
{
public:

  /**
   * Get the decoder that can decode the information in this Demuxer stream.
   */
  virtual Decoder* getDecoder();

  /**
   * Get the Demuxer this DemuxerStream belongs to.
   */
  virtual Demuxer* getDemuxer();

#ifndef SWIG
  static DemuxerStream*
  make(Container* container, int32_t index);
#endif

protected:
  DemuxerStream(Container* container, int32_t index);
  virtual
  ~DemuxerStream();
private:
  io::humble::ferry::RefPointer<Decoder> mDecoder;
};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* DEMUXERSTREAM_H_ */
