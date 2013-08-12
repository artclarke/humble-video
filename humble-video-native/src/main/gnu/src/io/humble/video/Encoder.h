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
 * Encoder.h
 *
 *  Created on: Jul 23, 2013
 *      Author: aclarke
 */

#ifndef ENCODER_H_
#define ENCODER_H_

#include <io/humble/video/Coder.h>
#include <io/humble/video/MediaPacket.h>
#include <io/humble/video/MediaAudio.h>
#include <io/humble/video/MediaPicture.h>
#include <io/humble/video/MediaSubtitle.h>

namespace io {
namespace humble {
namespace video {

class Encoder : public io::humble::video::Coder
{
public:
  /**
   * Create a Encoder that will use the given Codec.
   *
   * @return a Encoder
   * @throws InvalidArgument if codec is null or codec cannot decode.
   */
  static Encoder* make(Codec* codec);

  /**
   * Creates a Encoder from a given Encoder
   * @return a Encoder
   * @throws InvalidArgument if src is null
   */
  static Encoder* make(Encoder* src);

  /**
   * Encode the given MediaPicture using this encoder.
   *
   * The MediaPicture will allocate a buffer to use internally for this, and
   * will free it when the frame destroys itself.
   *
   * Also, when done in order to flush the encoder, caller should call
   * this method passing in 0 (null) for frame to tell the encoder
   * to flush any data it was keeping a hold of.
   *
   * @param output [out] The packet to encode into.  It will point
   *     to a buffer allocated in the frame.  Caller should check MediaPacket.isComplete()
   *     after call to find out if we had enough information to encode a full packet.
   * @param frame [in/out] The frame to encode
   * @param suggestedBufferSize The suggested buffer size to allocate or -1 for choose ourselves.
   *        If -1 we'll allocate a buffer exactly the same size (+1) as the decoded frame
   *        with the guess that you're encoding a frame because you want to use LESS space
   *        than that.
   *
   * @ return >= 0 on success; <0 on error.
   */
  virtual int32_t encodeVideo(MediaPacket * output,
      MediaPicture * frame, int32_t suggestedBufferSize)=0;

  /**
   * Encode the given MediaAudio using this encoder.
   *
   * Callers should call this repeatedly on a set of samples until
   * we consume all the samples.
   *
   * Also, when done in order to flush the encoder, caller should call
   * this method passing in 0 (null) for samples to tell the encoder
   * to flush any data it was keeping a hold of.
   *
   * @param output [out] The packet to encode into.  It will point
   *          to a buffer allocated in the frame.  Caller should check MediaPacket.isComplete()
   *     after call to find out if we had enough information to encode a full packet.
   * @param samples [in] The samples to consume
   * @param sampleToStartFrom [in] Which sample you want to start with
   *          This is usually zero, but if you're using a codec that
   *          packetizes output with small number of samples, you may
   *          need to call encodeAudio repeatedly with different starting
   *          samples to consume all of your samples.
   *
   * @return number of samples we consumed when encoding, or negative for errors.
   */
  virtual int32_t encodeAudio(MediaPacket * output,
      MediaAudio* samples, int32_t sampleToStartFrom)=0;

  /**
   * Not final API yet; do not use.
   */
  virtual int32_t encodeSubtitle(MediaPacket* output,
      MediaSubtitle* subtitles)=0;

protected:
  Encoder(Codec*, const AVCodecContext* src);
  virtual
  ~Encoder();
private:
};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* ENCODER_H_ */
