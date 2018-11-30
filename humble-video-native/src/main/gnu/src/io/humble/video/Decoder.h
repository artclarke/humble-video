/*******************************************************************************
 * Copyright (c) 2014, Andrew "Art" Clarke.  All rights reserved.
 *   
 * This file is part of Humble-Video.
 *
 * Humble-Video is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Humble-Video is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
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

#include <io/humble/ferry/RefPointer.h>
#include <io/humble/video/Coder.h>
#include <io/humble/video/Processor.h>
#include <io/humble/video/MediaPacket.h>
#include <io/humble/video/MediaAudio.h>
#include <io/humble/video/MediaPicture.h>
#include <io/humble/video/MediaSubtitle.h>

namespace io {
namespace humble {
namespace video {

/**
 * Decodes MediaPacket objects into MediaAudio, MediaPicture or MediaSubtitle objects.
 */
class VS_API_HUMBLEVIDEO Decoder :
  virtual public io::humble::video::Coder,
  virtual public io::humble::video::ProcessorEncodedSink,
  virtual public io::humble::video::ProcessorRawSource
{
public:
  /**
   * Create a Decoder that will use the given Codec.
   *
   * @return a Decoder
   * @throws InvalidArgument if codec is null or codec cannot decode.
   */
  static Decoder* make(Codec* codec);

  /**
   * Creates a Decoder, copying parameters from a given Coder (either an encoder or a decoder).
   * @return a Decoder
   * @throws InvalidArgument if src is null
   */
  static Decoder* make(Coder* src);

#ifndef SWIG
  static Decoder* make(const AVCodec* codec, const AVCodecParameters *src);
#endif // SWIG

  /**
   * Send a packet to the decoder for processing.
   * <p>
   * If the packet is not null it must be complete, and it is up to
   * the caller to ensure the Packet is valid (e.g. send a video packet
   * to a video decoder) for this Decoder. Otherwise exceptions shall
   * be thrown. If null, this instructs the Decoder to start flushing
   * any saved data (and usually means no more data shall come).
   * </p>
   * <p>
   * If ProcessorResult.RESULT_AWAITING_DATA is returned, then
   * the internal buffers may be full, and #receiveRaw(MediaRaw)
   * should be called to process some data.
   * </p>
   *
   * @param packet The packet. If null, the Decoder should flush any data.
   *
   * @return The ProcessorResult.
   * @see #sendEncoded
   */
  virtual ProcessorResult sendPacket(MediaPacket* packet);
  virtual ProcessorResult sendEncoded(MediaEncoded* media) {
    MediaPacket* m = dynamic_cast<MediaPacket*>(media);
    if (media && !m)
      throw io::humble::ferry::HumbleRuntimeError("expected MediaPacket object");
    else
      return sendPacket(m);
  }

  /**
   * Decode data into this output.
   * <p>
   * The caller is responsible for allocating the
   * correct underlying Media object.  This function will overwrite
   * any data in the output object.
   * </p>
   * <p>
   * If ProcessorResult.RESULT_AWAITING_DATA is returned, then
   * the internal buffers may be empty (i.e. we need data), and
   * more data should be passed in via #sendEncoded(MediaEncoded)
   * should be called to add some data.
   * </p>
   * <p>
   * If ProcessorResult.RESULT_END_OF_STREAM is returned, the Decoder
   * has been flushed and has no more data. If you get more data,
   * you'll need to make a new Decoder.
   * </p>
   * @param output The Media we decode to.
   *
   * @return The ProcessorResult
   *
   * @throws InvalidArgument if the media type is not compatible with this decoder.
   * @see #sendPacket
   */
  virtual ProcessorResult receiveRaw(MediaRaw* output);

protected:
  Decoder(const AVCodec* codec, const AVCodecParameters* src);
  virtual ~Decoder();

private:
  int64_t rebase(int64_t ts, MediaPacket* packet);
  io::humble::ferry::RefPointer<MediaRaw> mCachedMedia;
  int64_t mAudioDiscontinuityStartingTimeStamp;
  int64_t mSamplesSinceLastTimeStampDiscontinuity;
};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* DECODER_H_ */
