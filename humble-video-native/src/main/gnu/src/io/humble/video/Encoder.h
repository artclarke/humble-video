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
#include <io/humble/video/FilterGraph.h>
#include <io/humble/video/FilterAudioSource.h>
#include <io/humble/video/FilterAudioSink.h>

namespace io {
namespace humble {
namespace video {

/**
 * Encodes MediaAudio, MediaPicture or MediaSubtitle objects into MediaPacket objects.
 */
class VS_API_HUMBLEVIDEO Encoder : public io::humble::video::Coder
{
public:

  /**
   * the number of media object the encoder had to drop (i.e. skip
   * encoding) in order to ensure that time stamp values are
   * monotonically increasing.  See https://code.google.com/p/xuggle/issues/detail?id=180
   * for details on why this is.
   */
  virtual int64_t getNumDroppedFrames() { return mNumDroppedFrames; }

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
  static Encoder* make(Coder* src);

#ifndef SWIG
  static Encoder* make(Codec* codec, AVCodecContext* src);
#endif // ! SWIG

  /**
   * Open this Coder, using the given bag of Codec-specific options.
   *
   * @param inputOptions If non-null, a bag of codec-specific options.
   * @param unsetOptions If non-null, the bag will be emptied and then filled with
   *                     the options in <code>inputOptions</code> that were not set.
   *
   */
  virtual void open(KeyValueBag* inputOptions, KeyValueBag* unsetOptions);

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
   * @param output [out] The packet to encode into.  Caller should check
   *       MediaPacket.isComplete() after call to find out if we had enough
   *       information to encode a full packet.
   * @param picture [in] The picture to encode
   *
   * Note: caller must ensure that output has sufficient space to
   *   contain a full packet. Alas, there is currently no way to
   *   query an encoder to find out the maximum packet size that
   *   can be output (bummer, I know). That leaves the caller two
   *   options. (1) You can call Packet.make() before each encode
   *   call, and then the encoder will automagically create the correct
   *   sized buffer for that call (but if you reuse the packet, it
   *   may be too small for the next caller). Or (2) you  can call
   *   Packet.make(int) with a value that will be larger than your
   *   max packet size (in which case you can reuse the packet).
   */
  virtual void encodeVideo(MediaPacket * output,
      MediaPicture * picture);

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
   * @param output [out] The packet to encode into.  Caller should check
   *       MediaPacket.isComplete() after call to find out if we had enough
   *       information to encode a full packet.
   * @param samples [in] The samples to consume
   *
   * Note: caller must ensure that output has sufficient space to
   *   contain a full packet. Alas, there is currently no way to
   *   query an encoder to find out the maximum packet size that
   *   can be output (bummer, I know). That leaves the caller two
   *   options. (1) You can call Packet.make() before each encode
   *   call, and then the encoder will automagically create the correct
   *   sized buffer for that call (but if you reuse the packet, it
   *   may be too small for the next caller). Or (2) you  can call
   *   Packet.make(int) with a value that will be larger than your
   *   max packet size (in which case you can reuse the packet).
   *
   */
  virtual void encodeAudio(MediaPacket * output,
      MediaAudio* samples);

  /**
   * Encode the given Media using this encoder.
   *
   * Callers should call this repeatedly on a media object ntil
   * we consume all the media.
   *
   * Also, when done in order to flush the encoder, caller should call
   * this method passing in 0 (null) for media to tell the encoder
   * to flush any data it was keeping a hold of.
   *
   * @param output [out] The packet to encode into.  Caller should check
   *       MediaPacket.isComplete() after call to find out if we had enough
   *       information to encode a full packet.
   * @param samples [in] The media to consume
   *
   * Note: caller must ensure that output has sufficient space to
   *   contain a full packet. Alas, there is currently no way to
   *   query an encoder to find out the maximum packet size that
   *   can be output (bummer, I know). That leaves the caller two
   *   options. (1) You can call Packet.make() before each encode
   *   call, and then the encoder will automagically create the correct
   *   sized buffer for that call (but if you reuse the packet, it
   *   may be too small for the next caller). Or (2) you  can call
   *   Packet.make(int) with a value that will be larger than your
   *   max packet size (in which case you can reuse the packet).
   *
   * @throws throws an exception if getCodecType() and the underlying
   * media object do not align (e.g. if you pass in a MediaPicture but the
   * CodecType is audio data).
   *
   */
  virtual void encode(MediaPacket * output,
      MediaSampled* media);
#if 0
#ifndef SWIG
  virtual int32_t acquire();
  virtual int32_t release();
#endif // ! SWIG
#endif // 0

protected:
  virtual void checkOptionsBeforeOpen();
  Encoder(Codec*, AVCodecContext* src, bool copySrc);
  virtual
  ~Encoder();
private:
  // Used to ensure we have the right frame-size for codecs that
  // require fixed frame sizes on audio.
  io::humble::ferry::RefPointer<FilterGraph> mAudioGraph;
  io::humble::ferry::RefPointer<FilterAudioSource> mAudioSource;
  io::humble::ferry::RefPointer<FilterAudioSink> mAudioSink;
  io::humble::ferry::RefPointer<MediaAudio> mFilteredAudio;

  int64_t mLastPtsEncoded;
  int64_t mNumDroppedFrames;
};

} /* namespace video */
} /* namespace humble */
} /* namespace io */
#endif /* ENCODER_H_ */
