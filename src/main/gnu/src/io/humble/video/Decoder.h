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
 * Decoder.h
 *
 *  Created on: Jul 23, 2013
 *      Author: aclarke
 */

#ifndef DECODER_H_
#define DECODER_H_

#include <io/humble/video/Coder.h>
#include <io/humble/video/MediaPacket.h>
#include <io/humble/video/MediaAudio.h>
#include <io/humble/video/MediaPicture.h>
#include <io/humble/video/MediaSubtitle.h>

namespace io {
namespace humble {
namespace video {

class VS_API_HUMBLEVIDEO Decoder : public io::humble::video::Coder
{
public:
  /**
   * Create a {@link Decoder} that will use the given {@link Codec}.
   *
   * @return a {@link Decoder}
   * @throws InvalidArgument if codec is null or codec cannot decode.
   */
  static Decoder* make(Codec* codec);

  /**
   * Creates a {@link Decoder} from a given {@link Decoder}
   * @return a {@link Decoder}
   * @throws InvalidArgument if src is null
   */
  static Decoder* make(Decoder* src);

  /**
   * Flush this {@link Decoder}, getting rid of any cached packets (call after seek).
   * Next packet given to decode should be a key packet.
   */
  virtual void flush()=0;

  /**
   * Decode this packet into output.  It will
   * try to fill up the audio samples object, starting
   * from the byteOffset inside this packet.
   * <p>
   * The caller is responsible for allocating the
   * MediaAudio object.  This function will overwrite
   * any data in the samples object.
   * </p>
   * @param output The MediaAudio we decode to. Caller must check if it is complete on return.
   * @param packet    The packet we're attempting to decode from.
   * @param byteOffset Where in the packet payload to start decoding
   *
   * @return number of bytes actually processed from the packet, or negative for error
   */
  virtual int32_t decodeAudio(MediaAudio * output,
      MediaPacket *packet, int32_t byteOffset)=0;

  /**
   * Decode this packet into output.
   *
   * The caller is responsible for allocating the
   * MediaPicture object.  This function will potentially
   * overwrite any data in the frame object, but
   * you should pass the same MediaPicture into this function
   * repeatedly until Media.isComplete() is true.
   *
   * @param output The MediaPicture we decode. Caller must check if it is complete on return.
   * @param packet  The packet we're attempting to decode from.
   * @param byteOffset Where in the packet payload to start decoding
   *
   * @return number of bytes actually processed from the packet, or negative for error
   */
  virtual int32_t decodeVideo(MediaPicture * output,
      MediaPacket *packet, int32_t byteOffset)=0;

  /**
   * Decode this packet into output.
   *
   * The caller is responsible for allocating the
   * MediaPicture object.  This function will potentially
   * overwrite any data in the frame object, but
   * you should pass the same MediaPicture into this function
   * repeatedly until Media.isComplete() is true.
   *
   * @param output The MediaPicture we decode. Caller must check if it is complete on return.
   * @param packet  The packet we're attempting to decode from.
   * @param byteOffset Where in the packet payload to start decoding
   *
   * @return number of bytes actually processed from the packet, or negative for error
   */
  virtual int32_t decodeSubtitle(MediaSubtitle * output,
      MediaPacket *packet, int32_t byteOffset)=0;


protected:
  Decoder();
  virtual
  ~Decoder();
};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* DECODER_H_ */
